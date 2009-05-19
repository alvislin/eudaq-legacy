#ifndef EUDAQ_INCLUDED_StandardEvent
#define EUDAQ_INCLUDED_StandardEvent

#include "eudaq/Event.hh"
#include <vector>

namespace eudaq {

  class StandardPlane : public Serializable {
  public:
    StandardPlane(Deserializer &);
    void Serialize(Serializer &) const;
    typedef double pixel_t;
    StandardPlane(size_t pixels = 0, size_t frames = 1);

    size_t m_xsize, m_ysize;
    std::vector<unsigned> m_x, m_y;
    std::vector<std::vector<pixel_t> > m_pix;
    std::vector<bool> m_pivot;
  };

  class StandardEvent : public Event {
    EUDAQ_DECLARE_EVENT(StandardEvent);
  public:
    StandardEvent(unsigned run = 0, unsigned evnum = 0);
    StandardEvent(Deserializer &);
    void SetTimestamp(unsigned long long);
    void AddPlane(const StandardPlane &);
    size_t NumPlanes() const;
    const StandardPlane & GetPlane(size_t i) const;
    StandardPlane & GetPlane(size_t i);
    virtual void Serialize(Serializer &) const;
    virtual void Print(std::ostream &) const;
    virtual std::string GetType() const { return "StandardEvent"; }
  private:
    std::vector<StandardPlane> m_planes;
  };

}

#endif // EUDAQ_INCLUDED_StandardEvent
