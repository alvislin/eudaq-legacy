#ifndef EUDAQ_TIMEPIX_BORE_H
#define EUDAQ_TIMEPIX_BORE_H

// the svn revision of the corresponding tpccondata class
// unfirtunatelz this has to be updated manually :-(
#define SVN_REVISION 1853
#define PIXELMAN_FOR_EUDAQ

#include "eudaq/Event.hh"
#include "eudaq/RawDataEvent.hh"
#include <string>

// The common.h comes from pixelman
// Is is only needed for the special pixelman constructor
#ifdef PIXELMAN_FOR_EUDAQ
  #include "common.h"
#endif

namespace eudaq {

/**
 *  At the begin of a run the Pixelman writes sends the timepix config in a BORE.
 *  As the size of the data structures varies in size, depeding on the type of chip
 *  and the number of chips, it does not exist as one memory block.
 *  This class provides a collection of the required variables as well as a 
 *  serialiser and deserialiser.
 *
 *  The constructor which gets the Pixelman specific data structures is only 
 *  compiled in if the PIXELMAN_FOR_EUDAQ preprocessor flag is set. It is not needed
 *  to deserialise the class and thus the main Eudaq does not have a Pixelman dependency.
 */
class TimePixBore: public Event
{
 public:
        // The constructor used by the pixelman producer.
        // Only available from within pixelman

#ifdef PIXELMAN_FOR_EUDAQ
  explicit TimepixBore( unsigned int run, DevInfo* param, double timeToEndOfShutter, 
			double shutterLength, int moduleID,
			DACTYPE *dacVals, size_t sizeOfDacVals);
#endif

	/// the constructor used by the data collector. It gets a data stream from the deserializer
	explicit TimePixBore(Deserializer &);

	/// due to the inhomogeneous data structure this thing needs it's own serialise function
	virtual void Serialize (Serializer &) const;
	
	unsigned int GetRevision(){ return m_revision; }
	int GetNChips(){ return m_rowLen * m_numberOfRows; }
	int GetMPXType(){ return  m_mpxType; }
	std::string GetChipBoardIDString(){ return m_chipboardIDstring; }
	int GetModuleID(){ return m_moduleID; }
	std::string GetInterfaceName(){ return m_ifaceName; }
	double GetClock(){ return m_clockTimepix; }
	double GetTimeToEndOfShutter(){ return m_timeToEndOfShutter; }
	double GetShutterLength(){ return  m_shutterLength; }
	std::vector<unsigned short> GetDACValues(){ return m_dacVals; }
	//void Print(std::ostream & os) {RawDataEvent::Print(std::ostream & os)};



 protected:

	unsigned int m_revision; ///< the MarlinTPC svn revision of the correconding 
	                         ///< TPCCondData class
	int m_rowLen;            ///< number of chips per row
	int m_numberOfRows;      ///< number of rows in which chips are aligned
	                         ///< (e.g. quad has 4 chips, which are in 2 rows)
	int m_mpxType;           ///< medipix type - MPX_ORIG, MPX_MXR, MPX_TPX
	std::string m_chipboardIDstring; ///< id string of chip/chipboard
	std::string m_ifaceName;      ///< name oof the readout interface
	double m_clockTimepix;        ///< the timepix clock
	double m_timeToEndOfShutter;  ///< time from trigger to end of shutter
	double m_shutterLength; ///< length of the shutter window
	int m_moduleID;          ///< the module ID (LCIO CellID1)
	std::vector<unsigned short> m_dacVals; ///< the dac setting of all chips

};

} // namespace eudaq

#endif // EUDAQ_TIMEPIX_BORE_H