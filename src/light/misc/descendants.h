template<typename Type>
void getDescendantsOfType(std::vector<Type*>& dst, Node& parent, const String& typeName) {
    Array children = parent.get_children();
    for (std::size_t i = 0; i < children.size(); ++i) {
        Node2D* child = Object::cast_to<Node2D>(children[i]);
        if (child && child->is_visible()) {
            if(child->get_class() == typeName) {
                dst.push_back(Object::cast_to<Type>(child));
            }
            getDescendantsOfType<Type>(dst, *child, typeName);
        }
    }
}

template<typename Type>
std::vector<Type*> getDescendantsOfType(Node& parent, const String& typeName) {
    std::vector<Type*> descendantsOfType;
    getDescendantsOfType(descendantsOfType, parent, typeName);
    return descendantsOfType;
}