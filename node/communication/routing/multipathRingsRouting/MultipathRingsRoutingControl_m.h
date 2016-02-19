//
// Generated file, do not edit! Created by nedtool 4.6 from node/communication/routing/multipathRingsRouting/MultipathRingsRoutingControl.msg.
//

#ifndef _MULTIPATHRINGSROUTINGCONTROL_M_H_
#define _MULTIPATHRINGSROUTINGCONTROL_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>node/communication/routing/multipathRingsRouting/MultipathRingsRoutingControl.msg:13</tt> by nedtool.
 * <pre>
 * enum multipathRingsRoutingControlDef
 * {
 * 
 *     MPRINGS_NOT_CONNECTED = 1;
 *     MPRINGS_CONNECTED_TO_TREE = 2;
 *     MPRINGS_TREE_LEVEL_UPDATED = 3;
 * }
 * </pre>
 */
enum multipathRingsRoutingControlDef {
    MPRINGS_NOT_CONNECTED = 1,
    MPRINGS_CONNECTED_TO_TREE = 2,
    MPRINGS_TREE_LEVEL_UPDATED = 3
};

/**
 * Class generated from <tt>node/communication/routing/multipathRingsRouting/MultipathRingsRoutingControl.msg:19</tt> by nedtool.
 * <pre>
 * message MultipathRingsRoutingControlMessage
 * {
 *     int multipathRingsRoutingControlMessageKind @enum(multipathRingsRoutingControlDef);
 *     int sinkID;
 *     int level;
 * }
 * </pre>
 */
class MultipathRingsRoutingControlMessage : public ::cMessage
{
  protected:
    int multipathRingsRoutingControlMessageKind_var;
    int sinkID_var;
    int level_var;

  private:
    void copy(const MultipathRingsRoutingControlMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MultipathRingsRoutingControlMessage&);

  public:
    MultipathRingsRoutingControlMessage(const char *name=NULL, int kind=0);
    MultipathRingsRoutingControlMessage(const MultipathRingsRoutingControlMessage& other);
    virtual ~MultipathRingsRoutingControlMessage();
    MultipathRingsRoutingControlMessage& operator=(const MultipathRingsRoutingControlMessage& other);
    virtual MultipathRingsRoutingControlMessage *dup() const {return new MultipathRingsRoutingControlMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getMultipathRingsRoutingControlMessageKind() const;
    virtual void setMultipathRingsRoutingControlMessageKind(int multipathRingsRoutingControlMessageKind);
    virtual int getSinkID() const;
    virtual void setSinkID(int sinkID);
    virtual int getLevel() const;
    virtual void setLevel(int level);
};

inline void doPacking(cCommBuffer *b, MultipathRingsRoutingControlMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MultipathRingsRoutingControlMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef _MULTIPATHRINGSROUTINGCONTROL_M_H_

