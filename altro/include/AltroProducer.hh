#include "eudaq/Producer.hh"
#include <pthread.h>

namespace gear{

namespace altroproducer{

/** Nested command class, base class for all the commands */
class Command
{
public:
//    /** The available commands that the command receiver can pass to the readout loop
//     *  (runhandler).
//     *  All commands of the ilcserver--readout communication must be implemneted.
//     *  \li \c *CLI: No needed, producer is a class instance in the same executable
//     *  \li \c *UPD: FIXME: I don't know what this does
//     *  \li \c *POW: POWER
//     *  \li \c *PCA: PCA
//     *  \li \c *START: START_DAQ
//     *  \li \c *STOP:  STOP_DAQ
//     *  \li \c *STATUS: STATUS
//     *  \li \c *SOR: START_RUN
//     *  \li \c *CONT: CONTINUE_RUN 
//     *  \li \c *PAUSE: PAUSE_RUN
//     *  \li \c *EOR: END_RUN
//     *  \li \c SIGUSR1: Not needed, used to generate a status
//     *  \li \c SIGUSR2: TERMINATE
//     *
//     */
//    enum CommandTypes{ NONE = 0, START_RUN = 1 , STOP_RUN, STATUS, TERMINATE, RESET,
//		       START_DAQ, STOP_DAQ, POWER, PCA, PAUSE_RUN, CONTINUE_RUN  };
// 
//   CommandTypes commandType;

    /** Execute the command. This is the purely virtual function which has to be implemented 
     *  
     */
    virtual void Execute(RUNSTATUS *rs) = 0;
}

class Power : public Command
{
pulic:
    Power( int powercommand ); ///< command can be status (0), on (1) or off (2)
    virtual ~Power(){}
    
    virtual void Execute(RUNSTATUS *rs);

protected:
    int _powercommand;
};

class PCA : public Command
{
pulic:
    PCA( all the parameters) 
    virtual ~PCA(){}
    
    virtual void Execute(RUNSTATUS *rs, AltroProducer *producer);
};

class PCA : public Command
{
pulic:
    PCA( all the parameters) 
    virtual ~PCA(){}
    
    virtual void Execute(RUNSTATUS *rs, AltroProducer *producer);
};

class StartDAQ : public Command
{
pulic:
    StartDAQ( int control, int mode, int type ) 
    virtual ~StartDAQ(){}
    
    virtual void Execute(RUNSTATUS *rs, AltroProducer *producer);

protected:
    int _control, _mode, _type ;
};

class StopDAQ : public Command
{
pulic:
    StopDAQ() {}
    virtual ~StopDAQ(){}
    
    virtual void Execute(RUNSTATUS *rs,  AltroProducer *producer);
};

class Status : public Command
{
pulic:
    Status( all the parameters) 
    virtual ~Status(){}
    
    virtual void Execute(RUNSTATUS *rs,  AltroProducer *producer);
};

class StartRun : public Command
{
pulic:
    StartRun( unsigned int monevents, int logging, int type, 
		       int maxevents = 0, int maxmonevents = 0) 
    virtual ~StartRun(){}
    
    virtual void Execute(RUNSTATUS *rs);

    protected;
    unsigned int monevents;
    int logging, type, maxevents, maxmonevents;
};

class ContinueRun : public Command
{
pulic:
    ContinueRun( all the parameters) 
    virtual ~ContinueRun(){}
    
    virtual void Execute(RUNSTATUS *rs);
};

class PauseRun : public Command
{
pulic:
    PauseRun( all the parameters) 
    virtual ~PauseRun(){}
    
    virtual void Execute(RUNSTATUS *rs);
};

class EndRun : public Command
{
pulic:
    EndRun( all the parameters) 
    virtual ~EndRun(){}
    
    virtual void Execute(RUNSTATUS *rs = 0);
};

class Terminate : public Command
{
pulic:
    Terminate( all the parameters) 
    virtual ~Terminate(){}
    
    virtual void Execute(RUNSTATUS *rs = 0);
};

/** The producer class for the Altro RORC readout */
class AltroProducer : public eudaq::Producer
{
  public:
    AltroProducer(const std::string & name, const std::string & runcontrol);
    virtual ~AltroProducer();

    /** Send an recorded event from the physmem.
     *  The length is the number of unsigned long words.
     *  This function ensures little endian raw data. Use SendEvent to skip 
     *  the conversion, which is faster (memory does not have to be allocated and
     *  released).
     */
    void Event(volatile unsigned long *altrodata, int length);

    /** Threadsave version to set the m_run variable
     */
    void SetRunNumber(unsigned int runnumber);

    /** Threadsave version to get (a copy of) the m_run variable
     */
    unsigned int GetRunNumber();

    /** Threadsave version to set the m_event variable
     */
    void SetEventNumber(unsigned int eventnumber);

    /** Threadsave version to get (a copy of) the m_event variable
     */
    unsigned int GetEventNumber();

    /** Threadsave version to get (a copy of) the m_runactive variable
     */
    bool GetRunActive();

    /** Threadsave version to set the m_runactive variable
     */
    void SetRunActive(bool activestatus);

    /** Threadsave version to get (a copy of) the m_event variable.
     *  After creating the copy m_event is increased before releasing the 
     *  mutex. The non-increased version is returned (like in the post-increment operator).
     */
    unsigned int GetIncreaseEventNumber();

    /** The protected member functions live in the communication thread.
     *  Make sure they do not perfom computing intensive tasks and return as
     *  soon as possible.
     */
protected:
    virtual void OnConfigure(const eudaq::Configuration & param);
    virtual void OnPrepareRun(unsigned param);
    virtual void OnStartRun(unsigned param);
    virtual void OnStopRun();
    virtual void OnTerminate();
    virtual void OnReset();
    virtual void OnStatus();
    virtual void OnUnrecognised(const std::string & cmd, const std::string & param);

public:


    /** As the command receiver runs in a separate thread we need a queue to ensure all 
     *  commands are executed. Do not access directly, only use the thread safe CommandPush() and
     *  CommandPop()!
     */
    std::queue<Command> m_commandQueue;

    /// A mutex to protext the CommandQueue
    pthread_mutex_t m_commandqueue_mutex;

    /// The thread safe way to push a command to the command queue
    void CommandPush(Command c);

    /** The thread safe way to pop a command fom the command queue. If the queue is emty
     *  it returns Commands::NONE.
     */
    Command CommandPop();

    /** The thread safe way to get a reference to the first command fom the command queue
     *  without removing it from the queue
     */
    Command CommandFront();
    
    // all data members have to be protected by mutex since they can be accessed by multiple 
    // threads
    /// status wether a data run is active							
    bool m_runactive; pthread_mutex_t m_runactive_mutex;
    /// The run number
    unsigned m_run;  pthread_mutex_t m_run_mutex;
    /// The event number
    unsigned m_ev;   pthread_mutex_t m_ev_mutex;
};

}// namespace gear

}// namespace altroproducer
