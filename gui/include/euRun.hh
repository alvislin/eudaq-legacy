#include "ui_euRun.h"
#include "RunControlModel.hh"
#include "eudaq/RunControl.hh"
#include <QMainWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <QItemDelegate>
#include <QDir>
#include <QPainter>
#include <QTimer>

// To make Qt behave on OSX (to be checked on other OSes)
#define MAGIC_NUMBER 22

class RunConnectionDelegate : public QItemDelegate {
public:
  RunConnectionDelegate(RunControlModel * model);
private:
  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  RunControlModel * m_model;
};

class RunControlGUI : public QMainWindow, public Ui::wndRun, public eudaq::RunControl {
  Q_OBJECT
public:
  RunControlGUI(const std::string & listenaddress,
                QRect geom,
                QWidget *parent = 0,
                Qt::WindowFlags flags = 0);
private:
  virtual void OnConnect(const eudaq::ConnectionInfo & id) {
    static bool registered = false;
    if (!registered) {
      qRegisterMetaType<QModelIndex>("QModelIndex");
      registered = true;
    }
    //QMessageBox::information(this, "EUDAQ Run Control",
    //                         "This will reset all connected Producers etc.");
    m_run.newconnection(id);
    if (id.GetType() == "DataCollector") {
      emit StatusChanged("RUN", ("(" + eudaq::to_string(m_runnumber) + ")").c_str());
    }
  }
  virtual void OnDisconnect(const eudaq::ConnectionInfo & id) {
    m_run.disconnected(id);
  }
  virtual void OnReceive(const eudaq::ConnectionInfo & id, counted_ptr<eudaq::Status> status) {
    static bool registered = false;
    if (!registered) {
      qRegisterMetaType<QModelIndex>("QModelIndex");
      registered = true;
    }
    if (id.GetType() == "DataCollector") {
      emit StatusChanged("EVENT", status->GetTag("EVENT").c_str());
    } else if (id.GetType() == "Producer" && id.GetName() == "TLU") {
      emit StatusChanged("TRIG", status->GetTag("TRIG").c_str());
      emit StatusChanged("TIMESTAMP", status->GetTag("TIMESTAMP").c_str());
    }
    m_run.SetStatus(id, *status);
  }
  void closeEvent(QCloseEvent * event) {
    if (m_run.rowCount() > 0 &&
        QMessageBox::question(this, "Quitting", "Terminate all connections and quit?",
                              QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) {
      event->ignore();
    } else {
      Terminate();
      event->accept();
    }
  }
private slots:
  void on_btnConfig_clicked() {
    std::string settings = cmbConfig->currentText().toStdString();
    Configure(settings);
  }
  void on_btnReset_clicked() {
    Reset();
  }
  void on_btnStart_clicked() {
    StartRun(txtRunmsg->displayText().toStdString());
    emit StatusChanged("RUN", eudaq::to_string(m_runnumber).c_str());
  }
  void on_btnStop_clicked() {
    StopRun();
    StatusChanged("RUN", ("(" + eudaq::to_string(m_runnumber) + ")").c_str());
  }
  void on_btnLog_clicked() {
    std::string msg = txtLogmsg->displayText().toStdString();
    EUDAQ_USER(msg);
  }
  void timer() {
    GetStatus();
  }
  void ChangeStatus(const QString & name, const QString & value) {
    status_t::iterator i = m_status.find(name.toStdString());
    if (i != m_status.end()) {
      i->second.second->setText(value);
    }
  }
signals:
  void StatusChanged(const QString &, const QString &);
private:
  RunControlModel m_run;
  RunConnectionDelegate m_delegate;
  QTimer m_statustimer;
  typedef std::map<std::string, std::pair<QLabel *, QLabel *> > status_t;
  status_t m_status;
};
