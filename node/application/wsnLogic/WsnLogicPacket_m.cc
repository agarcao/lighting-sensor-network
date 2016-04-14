//
// Generated file, do not edit! Created by nedtool 4.6 from node/application/wsnLogic/WsnLogicPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "WsnLogicPacket_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

WsnLogicData::WsnLogicData()
{
    type = 0;
    originNodeID = 0;
    senderNodeID = 0;
    for (unsigned int i=0; i<8; i++)
        destinationNodesID[i] = 0;
    hop = 0;
}

void doPacking(cCommBuffer *b, WsnLogicData& a)
{
    doPacking(b,a.type);
    doPacking(b,a.originNodeID);
    doPacking(b,a.senderNodeID);
    doPacking(b,a.destinationNodesID,8);
    doPacking(b,a.hop);
}

void doUnpacking(cCommBuffer *b, WsnLogicData& a)
{
    doUnpacking(b,a.type);
    doUnpacking(b,a.originNodeID);
    doUnpacking(b,a.senderNodeID);
    doUnpacking(b,a.destinationNodesID,8);
    doUnpacking(b,a.hop);
}

class WsnLogicDataDescriptor : public cClassDescriptor
{
  public:
    WsnLogicDataDescriptor();
    virtual ~WsnLogicDataDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(WsnLogicDataDescriptor);

WsnLogicDataDescriptor::WsnLogicDataDescriptor() : cClassDescriptor("WsnLogicData", "")
{
}

WsnLogicDataDescriptor::~WsnLogicDataDescriptor()
{
}

bool WsnLogicDataDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<WsnLogicData *>(obj)!=NULL;
}

const char *WsnLogicDataDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int WsnLogicDataDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int WsnLogicDataDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *WsnLogicDataDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "type",
        "originNodeID",
        "senderNodeID",
        "destinationNodesID",
        "hop",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int WsnLogicDataDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    if (fieldName[0]=='o' && strcmp(fieldName, "originNodeID")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderNodeID")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationNodesID")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "hop")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *WsnLogicDataDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "unsigned short",
        "unsigned short",
        "unsigned short",
        "int",
        "int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *WsnLogicDataDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int WsnLogicDataDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicData *pp = (WsnLogicData *)object; (void)pp;
    switch (field) {
        case 3: return 8;
        default: return 0;
    }
}

std::string WsnLogicDataDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicData *pp = (WsnLogicData *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->type);
        case 1: return ulong2string(pp->originNodeID);
        case 2: return ulong2string(pp->senderNodeID);
        case 3: if (i>=8) return "";
                return long2string(pp->destinationNodesID[i]);
        case 4: return long2string(pp->hop);
        default: return "";
    }
}

bool WsnLogicDataDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicData *pp = (WsnLogicData *)object; (void)pp;
    switch (field) {
        case 0: pp->type = string2ulong(value); return true;
        case 1: pp->originNodeID = string2ulong(value); return true;
        case 2: pp->senderNodeID = string2ulong(value); return true;
        case 3: if (i>=8) return false;
                pp->destinationNodesID[i] = string2long(value); return true;
        case 4: pp->hop = string2long(value); return true;
        default: return false;
    }
}

const char *WsnLogicDataDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *WsnLogicDataDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicData *pp = (WsnLogicData *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(WsnLogicDataPacket);

WsnLogicDataPacket::WsnLogicDataPacket(const char *name, int kind) : ::ApplicationPacket(name,kind)
{
}

WsnLogicDataPacket::WsnLogicDataPacket(const WsnLogicDataPacket& other) : ::ApplicationPacket(other)
{
    copy(other);
}

WsnLogicDataPacket::~WsnLogicDataPacket()
{
}

WsnLogicDataPacket& WsnLogicDataPacket::operator=(const WsnLogicDataPacket& other)
{
    if (this==&other) return *this;
    ::ApplicationPacket::operator=(other);
    copy(other);
    return *this;
}

void WsnLogicDataPacket::copy(const WsnLogicDataPacket& other)
{
    this->extraData_var = other.extraData_var;
}

void WsnLogicDataPacket::parsimPack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimPack(b);
    doPacking(b,this->extraData_var);
}

void WsnLogicDataPacket::parsimUnpack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimUnpack(b);
    doUnpacking(b,this->extraData_var);
}

WsnLogicData& WsnLogicDataPacket::getExtraData()
{
    return extraData_var;
}

void WsnLogicDataPacket::setExtraData(const WsnLogicData& extraData)
{
    this->extraData_var = extraData;
}

class WsnLogicDataPacketDescriptor : public cClassDescriptor
{
  public:
    WsnLogicDataPacketDescriptor();
    virtual ~WsnLogicDataPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(WsnLogicDataPacketDescriptor);

WsnLogicDataPacketDescriptor::WsnLogicDataPacketDescriptor() : cClassDescriptor("WsnLogicDataPacket", "ApplicationPacket")
{
}

WsnLogicDataPacketDescriptor::~WsnLogicDataPacketDescriptor()
{
}

bool WsnLogicDataPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<WsnLogicDataPacket *>(obj)!=NULL;
}

const char *WsnLogicDataPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int WsnLogicDataPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int WsnLogicDataPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *WsnLogicDataPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "extraData",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int WsnLogicDataPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='e' && strcmp(fieldName, "extraData")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *WsnLogicDataPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "WsnLogicData",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *WsnLogicDataPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int WsnLogicDataPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicDataPacket *pp = (WsnLogicDataPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string WsnLogicDataPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicDataPacket *pp = (WsnLogicDataPacket *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getExtraData(); return out.str();}
        default: return "";
    }
}

bool WsnLogicDataPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicDataPacket *pp = (WsnLogicDataPacket *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *WsnLogicDataPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return opp_typename(typeid(WsnLogicData));
        default: return NULL;
    };
}

void *WsnLogicDataPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    WsnLogicDataPacket *pp = (WsnLogicDataPacket *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getExtraData()); break;
        default: return NULL;
    }
}


