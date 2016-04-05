//
// Generated file, do not edit! Created by nedtool 4.6 from node/application/wsnLogic/WsnLogicPacket.msg.
//

#ifndef _WSNLOGICPACKET_M_H_
#define _WSNLOGICPACKET_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "ApplicationPacket_m.h"
// }}

/**
 * Struct generated from node/application/wsnLogic/WsnLogicPacket.msg:22 by nedtool.
 */
struct WsnLogicData
{
    WsnLogicData();
    unsigned short type;
    unsigned short originNodeID;
    unsigned short senderNodeID;
    unsigned short destinationNodesID;
    int hop;
};

void doPacking(cCommBuffer *b, WsnLogicData& a);
void doUnpacking(cCommBuffer *b, WsnLogicData& a);

/**
 * Class generated from <tt>node/application/wsnLogic/WsnLogicPacket.msg:30</tt> by nedtool.
 * <pre>
 * packet WsnLogicDataPacket extends ApplicationPacket
 * {
 *     WsnLogicData extraData;
 * }
 * </pre>
 */
class WsnLogicDataPacket : public ::ApplicationPacket
{
  protected:
    WsnLogicData extraData_var;

  private:
    void copy(const WsnLogicDataPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const WsnLogicDataPacket&);

  public:
    WsnLogicDataPacket(const char *name=NULL, int kind=0);
    WsnLogicDataPacket(const WsnLogicDataPacket& other);
    virtual ~WsnLogicDataPacket();
    WsnLogicDataPacket& operator=(const WsnLogicDataPacket& other);
    virtual WsnLogicDataPacket *dup() const {return new WsnLogicDataPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual WsnLogicData& getExtraData();
    virtual const WsnLogicData& getExtraData() const {return const_cast<WsnLogicDataPacket*>(this)->getExtraData();}
    virtual void setExtraData(const WsnLogicData& extraData);
};

inline void doPacking(cCommBuffer *b, WsnLogicDataPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, WsnLogicDataPacket& obj) {obj.parsimUnpack(b);}


#endif // ifndef _WSNLOGICPACKET_M_H_

