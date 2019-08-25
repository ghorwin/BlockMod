#ifndef BM_EntityH
#define BM_EntityH

class QXmlStreamReader;
class QXmlStreamWriter;

#include <QString>

namespace BLOCKMOD {

/*! Abstract base class for all entities of the network. */
class Entity {
public:
	Entity() {}

	Entity(const QString & name);

	/*! Unique ID name of this entity. */
	QString			m_name;
};

} // namespace BLOCKMOD


#endif // BM_EntityH

