template<typename element>
class AbstractTree
{
public:
	//void addNode();
	//void TreeWalkDemo(&element);
	//void setRootElement();
	element& getRoot();
	void setRoot(element pass);
private:
	element root;

};
template<typename element>
typename inline element& AbstractTree<element>::getRoot() { return root; }
template<typename element>
typename inline void AbstractTree<element>::setRoot(element pass) { root = pass; }
