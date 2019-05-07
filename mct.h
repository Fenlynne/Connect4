#pragma once
class MCTState{
public:
    MCTState(){}

    virtual ~MCTState(){}

    virtual int DefaultPolicy() = 0;

    virtual MCTState* NextChildState() = 0;

    virtual void Print() = 0;
};

class MCTNode{

protected:
    MCTNode* first_child_, *parent_, *next_sibling_;

    MCTState* state_;

	float value_;
	int visit_times_;

public:
    MCTNode(MCTState* state, MCTNode* parent) : state_(state),parent_(parent){
		value_ = 0;
		visit_times_ = 0;
        first_child_ = next_sibling_ = nullptr;
    }

    virtual ~MCTNode(){
        for (MCTNode* child = first_child_, *next; child != nullptr; child = next){
            next = child->next_sibling_;
            delete child;
        }
        delete state_;
    }

    float GetValue() { return value_;}

    int GetVisitTimes() { return visit_times_;}

    void AddValue(int delta){
		switch (delta) {
		case -1:
			value_ -= 1; break;
		case 0:
			value_ -= 0.5; break;
		case 1:
			break;
		}
		++visit_times_;
	}

    virtual MCTNode* Expand() = 0;

    MCTNode* GetFirstChild() const {return first_child_;}

    MCTNode* GetNextSibling() const {return next_sibling_;}

    MCTNode* GetParent() const{ return parent_;}

    MCTState* GetState() const{ return state_;}

    void SetFirstChild(MCTNode* first_child){ first_child_ = first_child;}

    void SetNextSibling(MCTNode* next_sibling) { next_sibling_ =  next_sibling;}
};

class MCT{
protected:
    MCTNode* root_;
public:
    MCT(MCTNode* root):root_(root){}

    virtual ~MCT(){
		while (root_->GetParent() != nullptr) root_ = root_->GetParent();
		delete root_;
	}

    virtual MCTNode* TreePolicy() = 0;
};