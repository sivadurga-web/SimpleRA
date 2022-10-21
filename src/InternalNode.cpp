#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "RecordPtr.hpp"
#include <stack>
//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    // cout << "InternalNode::insert_key" << endl;

    this->load();
    stack <pair<NodeType, TreePtr>> st;
    st.push({this->node_type, this->tree_ptr});
    while (st.top().first == INTERNAL) {
        InternalNode* node_ptr = new InternalNode(st.top().second);
        TreePtr key_ptr= NULL_PTR;
        int i = 0;
        for (i = 0 ; i < node_ptr->size-1;i++) {
            if (key <= node_ptr->keys[i]) {
                key_ptr = node_ptr->tree_pointers[i];
                break;
            }
        }
        
        if (is_null(key_ptr)) {
            key_ptr = node_ptr->tree_pointers[i];
        }
        delete node_ptr;
        auto* tree_ptr = TreeNode::tree_node_factory(key_ptr);
        st.push({tree_ptr->node_type, key_ptr});
        delete tree_ptr;

    }
    TreeNode* root_node = TreeNode::tree_node_factory(st.top().second);
    
    st.pop();
    new_tree_ptr = root_node->insert_key(key, record_ptr);
    Key new_key = root_node->max();
    // cout << new_tree_ptr << " " << new_key << endl;
    while (st.size()) {
        
        if (!is_null(new_tree_ptr)){
            InternalNode* node_ptr = new InternalNode(st.top().second);
            vector<Key> keys;
            vector<TreePtr> tree_pointers;
            // cout << "Tree_ptr ---> "<<node_ptr->tree_ptr << endl;
            bool is_inserted = false;
            for (int i = 0 ; i < node_ptr->size-1 ; i++) {
                if (node_ptr->keys[i] < new_key || is_inserted) {
                    keys.push_back(node_ptr->keys[i]);
                    tree_pointers.push_back(node_ptr->tree_pointers[i]);
                }
                else if (is_inserted == false){
                    is_inserted = true;
                    keys.push_back(new_key);
                    keys.push_back(node_ptr->keys[i]);
                    tree_pointers.push_back(node_ptr->tree_pointers[i]);
                    tree_pointers.push_back(new_tree_ptr);
                }
            }
            
            tree_pointers.push_back(node_ptr->tree_pointers[node_ptr->size-1]);
            if (is_inserted == false) {
                keys.push_back(new_key);
                tree_pointers.push_back(new_tree_ptr);
                is_inserted = true;
            } 
            if (tree_pointers.size() <= FANOUT) {
                node_ptr->keys = keys;
                node_ptr->tree_pointers = tree_pointers;
                node_ptr->size++;
                
                node_ptr->dump();
                delete node_ptr;

                new_tree_ptr = NULL_PTR;
            }
            else {
                InternalNode* new_internal_node = new InternalNode();
                // new_internal_node->load();
                node_ptr->tree_pointers.clear();
                node_ptr->keys.clear();
                node_ptr->size = 0;
                int ind1 = 0;
                int ind2 = 0;
                while (ind2 < MIN_OCCUPANCY) {
                    node_ptr->tree_pointers.push_back(tree_pointers[ind2]);
                    node_ptr->size++;
                    ind2++;
                }
                while (ind1 < MIN_OCCUPANCY-1) {
                    node_ptr->keys.push_back(keys[ind1++]);
                }
                new_tree_ptr = new_internal_node->tree_ptr;
                new_key = node_ptr->max();
                // cout << new_tree_ptr << " "<< new_key << endl;
                while (ind2 < tree_pointers.size()) {
                    new_internal_node->tree_pointers.push_back(tree_pointers[ind2++]);
                    new_internal_node->size++;
                }
                ind1++;
                while (ind1 < keys.size()) {
                    new_internal_node->keys.push_back(keys[ind1++]);
                }
                new_internal_node->dump();
                node_ptr->dump();
                delete node_ptr;
                delete new_internal_node;

            }
        }
        else {
            return NULL_PTR;
        }
        st.pop();
    }
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    // cout << "InternalNode::delete_key" << endl;
    this->load();
    int tree_pointer_index = -1;
    for (int i = 0 ; i < this->keys.size() ; i++) {
        if (key <= this->keys[i]) {
            auto* child = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child->delete_key(key);
            tree_pointer_index = i;
            delete child;
            break;
        }
    }
    if (tree_pointer_index == -1) {
        auto* child = TreeNode::tree_node_factory(this->tree_pointers[this->size-1]);
        child->delete_key(key);
        tree_pointer_index = this->size-1;
        delete child;
    }
    // 1 - Check if the node has greater than b/2 or not
    auto* child = TreeNode::tree_node_factory(this->tree_pointers[tree_pointer_index]);
    if (child->size >= MIN_OCCUPANCY) {
        this->dump();
        return;
    }
    // ~~~~~~~~~~~~~ Underflow cases ~~~~~~~~~~~~~~~~
    // 2 - Redistributing, Merging with left sibling if exists and > MIN_OCCUPANCY
    if (tree_pointer_index != 0) { // Means left sibling exists
        
        if (child->node_type == LEAF) {
            auto* child_predecessor = TreeNode::tree_node_factory(this->tree_pointers[tree_pointer_index-1]);
            // Redistribution at leaf level
            //     -     Moving the maximum element in the left node to the right side 
            //     -     and changing the search key at current level.
            if (child_predecessor->size > MIN_OCCUPANCY) {
                LeafNode* child_leaf = new LeafNode(this->tree_pointers[tree_pointer_index]);
                LeafNode* child_leaf_p = new LeafNode(this->tree_pointers[tree_pointer_index - 1]);
                Key max_key = child_leaf_p->max();
                child_leaf->data_pointers[max_key] = child_leaf_p->data_pointers[max_key];
                child_leaf->size++;
                child_leaf_p->size--;
                child_leaf_p->data_pointers.erase(max_key);
                this->keys[tree_pointer_index-1] = child_leaf_p->max();
                this->dump();
                child_leaf->dump();
                child_leaf_p->dump();
                delete child_leaf;
                delete child_leaf_p;
                delete child;
                delete child_predecessor;
                return;
            }
            // Merging at leaf level 
            //     -    Merge the current leaf node with its left sibling 
            //     -    and update it's parent search keys and tree pointers
            else {
                LeafNode* child = new LeafNode(this->tree_pointers[tree_pointer_index]);
                LeafNode* child_p = new LeafNode(this->tree_pointers[tree_pointer_index-1]);
                for (auto x:child->data_pointers) {
                    child_p->data_pointers[x.first] = x.second;
                    child_p->size++;
                }
                delete_file(child->tree_ptr);
                this->tree_pointers.erase(this->tree_pointers.begin() + tree_pointer_index);
                if (this->keys.begin() + tree_pointer_index != this->keys.end())
                this->keys.erase(this->keys.begin()+ tree_pointer_index);
                this->keys[tree_pointer_index-1] = child_p->max();
                this->size--;
                child_p->next_leaf_ptr = child->next_leaf_ptr;
                this->dump();
                child_p->dump();
                delete child;
                delete child_p;
                return;
            }
        }
        else { 
            auto* child_predecessor = TreeNode::tree_node_factory(this->tree_pointers[tree_pointer_index-1]);
            // Redistribution at internal level
            //      -       Rotating the left max key and pointer to parent search key 
            //      -       Then insert the previous parent search key and in the first place of right child search keys
            //      -       Move the left child max tree pointer to the right child first pointer
            if (child_predecessor->size > MIN_OCCUPANCY) {
                InternalNode* child = new InternalNode(this->tree_pointers[tree_pointer_index]);
                InternalNode* child_p = new InternalNode(this->tree_pointers[tree_pointer_index - 1]);
                // preparing child node
                child->keys.insert(child->keys.begin(), this->keys[tree_pointer_index-1]);
                child->tree_pointers.insert(child->tree_pointers.begin(), child_p->tree_pointers[child_p->size-1]);
                child->size++;
                // preparing child_p node
                child_p->tree_pointers.pop_back();
                Key search_key = child_p->keys.back();
                child_p->keys.pop_back();
                child_p->size--;
                // updating the parent node search key
                this->keys[tree_pointer_index-1] = search_key;
                this->dump();
                child->dump();
                child_p->dump();
                delete child;
                delete child_p;
                return;
            }
            // Merging at internal level
            //      -       Take the search key between two childs and insert in the left child  keys at end
            //      -       and add all keys of child to left sibling,along with tree pointers.
            //      -       Removing the empty tree pointer and key value in the internal node 
            else {
                InternalNode* child = new InternalNode(this->tree_pointers[tree_pointer_index]);
                InternalNode* child_p = new InternalNode(this->tree_pointers[tree_pointer_index-1]);
                // Inserting search key, child keys in the child_p
                child_p->keys.push_back(this->keys[tree_pointer_index-1]);
                for (auto x:child->keys) {
                    child_p->keys.push_back(x);
                }
                for (auto x:child->tree_pointers) {
                    child_p->tree_pointers.push_back(x);
                    child_p->size++;
                }
                this->keys.erase(this->keys.begin() +  tree_pointer_index-1);
                this->tree_pointers.erase(this->tree_pointers.begin() + tree_pointer_index);
                this->size--;
                delete_file(child->tree_ptr);
                this->dump();
                child_p->dump();
                delete child;
                delete child_p;
                return;
            }
        }  


    }
    // 3 - Redistributing, Merging with right sibling if exists and > MIN_OCCUPANCY
    if (tree_pointer_index != FANOUT-1) { // Means right sibling exists
        
        if (child->node_type == LEAF) {
            auto* child_predecessor = TreeNode::tree_node_factory(this->tree_pointers[tree_pointer_index+1]);
            // Redistribution at leaf level
            //     -     Moving the minimum element in the right node to the left side 
            //     -     and changing the search key at current level.
            if (child_predecessor->size > MIN_OCCUPANCY) {
                LeafNode* child_leaf = new LeafNode(this->tree_pointers[tree_pointer_index]);
                LeafNode* child_leaf_p = new LeafNode(this->tree_pointers[tree_pointer_index + 1]);
                auto x  = child_leaf_p->data_pointers.begin();
                Key min_key = x->first;
                child_leaf->data_pointers[min_key] = child_leaf_p->data_pointers[min_key];
                child_leaf->size++;
                child_leaf_p->size--;
                child_leaf_p->data_pointers.erase(min_key);
                this->keys[tree_pointer_index] = child_leaf->max();
                this->dump();
                child_leaf->dump();
                child_leaf_p->dump();
                delete child_leaf;
                delete child_leaf_p;
                delete child;
                delete child_predecessor;
                return;
            }
            // Merging at leaf level 
            //     -    Merge the current leaf node with its right sibling 
            //     -    and update it's parent search keys and tree pointers
            else {
                LeafNode* child = new LeafNode(this->tree_pointers[tree_pointer_index]);
                LeafNode* child_p = new LeafNode(this->tree_pointers[tree_pointer_index+1]);
                for (auto x:child_p->data_pointers) {
                    child->data_pointers[x.first] = x.second;
                    child->size++;
                }
                delete_file(child_p->tree_ptr);
                this->tree_pointers.erase(this->tree_pointers.begin() + tree_pointer_index+1);
                if (this->keys.begin() + tree_pointer_index != this->keys.end())
                this->keys.erase(this->keys.begin()+ tree_pointer_index);
                // this->keys[tree_pointer_index] = child_p->max();
                this->size--;
                child->next_leaf_ptr = child_p->next_leaf_ptr;
                this->dump();
                child->dump();
                delete child;
                delete child_p;
                return;
            }
        }
        else { 
            auto* child_predecessor = TreeNode::tree_node_factory(this->tree_pointers[tree_pointer_index+1]);
            // Redistribution at internal level
            //      -       Rotating the right min key  to parent search key 
            //      -       Then insert the previous parent search key and in the last place of left child search keys
            //      -       Move the right child min tree pointer to the  child last pointer
            if (child_predecessor->size > MIN_OCCUPANCY) {
                InternalNode* child = new InternalNode(this->tree_pointers[tree_pointer_index]);
                InternalNode* child_p = new InternalNode(this->tree_pointers[tree_pointer_index + 1]);
                // preparing child node
                child->keys.push_back( this->keys[tree_pointer_index]);
                child->tree_pointers.push_back(child_p->tree_pointers[0]);
                child->size++;
                // preparing child_p node
                child_p->tree_pointers.erase(child_p->tree_pointers.begin());
                Key search_key = child_p->keys[0];
                child_p->keys.erase(child_p->keys.begin());
                child_p->size--;
                
                // updating the parent node search key
                this->keys[tree_pointer_index] = search_key;
                this->dump();
                child->dump();
                child_p->dump();
                delete child;
                delete child_p;
                return;
            }
            // Merging at internal level
            //      -       Take the search key between two childs and insert in the right child  keys at end
            //      -       and add all keys of right sibling to child,along with tree pointers.
            //      -       Take the maximum key of the left child obtained and update the tree_pointer_index key if exists
            else {
                InternalNode* child = new InternalNode(this->tree_pointers[tree_pointer_index]);
                InternalNode* child_p = new InternalNode(this->tree_pointers[tree_pointer_index+1]);
                // Inserting search key, child keys in the child_p
                child->keys.push_back(this->keys[tree_pointer_index]);
                for (auto x:child_p->keys) {
                    child->keys.push_back(x);
                }
                for (auto x:child_p->tree_pointers) {
                    child->tree_pointers.push_back(x);
                    child->size++;
                }
                this->keys.erase(this->keys.begin() +  tree_pointer_index);
                this->tree_pointers.erase(this->tree_pointers.begin() + tree_pointer_index+1);
                this->size--;
                delete_file(child->tree_ptr);
                this->dump();
                child->dump();
                delete child;
                delete child_p;
                return;
            }
        }  


    }

    // this->dump();
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++) {
        if (min_key <= this->keys[i]) {
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++) {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i - 1]) + " < x";
        } else {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    
    for (int i = 0; i < this->size - 1; i++) {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is) {
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++) {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
