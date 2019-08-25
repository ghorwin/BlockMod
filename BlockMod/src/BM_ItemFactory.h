#ifndef BM_EntityFactoryH
#define BM_EntityFactoryH

namespace BLOCKMOD {

/*! The instances of blocks, sockets or connectors are instantiated by the factory. */
class EntityFactory {
public:
	EntityFactory();
	virtual ~EntityFactory() {}

	/*! Instantiates a block by id name.
		\return Returns a base class pointer to the new Block instance, if typeIdentifier is invalid,
	*/
//	static Block * createBlock(const QString & typeIdentifier);
};

} // namespace BLOCKMOD

#endif // BM_EntityFactoryH
