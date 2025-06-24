#include <Eigen/Dense>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>

class Value : public std::enable_shared_from_this<Value>{
    public: 
        float data, grad, back;
        std::shared_ptr<Value> left, right;
        std::string op;

        Value() : data(0), grad(0) {}
        Value(float x) : data(x), grad(0), op("") {}

        // Arithmetic Operators
        std::shared_ptr<Value> operator+(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(other->data + this->data);
            res->op = "+";
            res->left = shared_from_this();
            res->right = other;
            // std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") " << res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }

        std::shared_ptr<Value> operator*(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(other->data * this->data);
            res->op = "*";
            res->left = shared_from_this();
            res->right = other;
            // std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") "<< res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }

        // ostream
        friend std::ostream& operator<<(std::ostream& out, const std::shared_ptr<Value>& obj){
            out << "Value(data= " << obj->data << " | grad= " << obj->grad << ")" << std::endl;
            return out;
        }
};
 
std::vector<std::shared_ptr<Value>> backward(std::shared_ptr<Value> head){
    std::vector<std::shared_ptr<Value>> topo;
    std::stack<std::shared_ptr<Value>> s;


    // DFS to build the topo sort order
    s.push(head);
    while ( !s.empty() ){
        std::shared_ptr<Value> temp = s.top();
        topo.push_back(temp);
        // std::cout << "Value(data= " << temp->data << " | grad= " << temp->grad << ") "<< std::endl;    
        s.pop();
        if( temp->left != NULL ){
            s.push(temp->left);
        }
        if( temp->right != NULL ){
            s.push(temp->right);
        }
    }
    // 
    head->grad = 1; // root of the tree needs grad = 1; (grad = 0 by default)
    for(auto it = topo.begin(); it != topo.end(); it++){
        auto v = *it;
        if( v->op == "+"){
            if ( v->left ) v->left->grad += 1 * v->grad;
            if ( v->right ) v->right->grad += 1 * v->grad;
        }
        else if( v->op == "*"){
            if ( v->left ) v->left->grad += v->right->data * v->grad;
            if ( v->right ) v->right->grad += v->left->data * v->grad;
        }
        std::cout << "Value(data= " << v->data << " | grad= " << v->grad << ") "<< std::endl;    
    }
    return topo;

}

// Wrappers 
// Addition
std::shared_ptr<Value> operator+(
    const std::shared_ptr<Value>& lhs,
    const std::shared_ptr<Value>& rhs) {
    return lhs->operator+(rhs);
}

std::shared_ptr<Value> operator+(const std::shared_ptr<Value>& lhs, float rhs) {
    return lhs->operator+(std::make_shared<Value>(rhs));
}

std::shared_ptr<Value> operator+(float lhs, const std::shared_ptr<Value>& rhs) {
    return std::make_shared<Value>(lhs)->operator+(rhs);
}

// Multiplication
std::shared_ptr<Value> operator*(
    const std::shared_ptr<Value>& lhs,
    const std::shared_ptr<Value>& rhs) {
    return lhs->operator*(rhs);
}

int main() {
    // Eigen::VectorXd x(2);
    // Eigen::MatrixXd A(2,2);
    // Eigen::VectorXd y(2);
    // x << 1, 1;
    // A << 1, 2,
    //      3, 4;
    // y = A*x;
    // std::cout << "Matrix:\n" << A << std::endl;
    // std::cout << "Matrix:\n" << x << std::endl;
    // std::cout << "Matrix:\n" << y << std::endl;

 

    auto a = std::make_shared<Value>(2);
    auto b = std::make_shared<Value>(-3);
    auto c = std::make_shared<Value>(10);
    auto f = std::make_shared<Value>(-2);

    auto e = a * b;      // e = a * b
    auto d = e + c;         // d = e + c
    auto L = d * f;         // L = d * f
    auto T = L + 1.0f;
    // L = ((a2*b-3)+c10) * f-2
    //              -8 L
    //           -2 f  *  4 d
    //                10 c  +  -6 e          
    //                      -3 b * 2 a
    // dL/dd = f, dL/df = d
    // dL/dc = dL/dd * dd/dc = f * 1
    // dL/de = dL/dd * dd/de = f * 1
    // dL/db = dL/de * de/db = (f ) * a
    // dL/da = dL/de * de/da = (f ) * b

    std::vector<std::shared_ptr<Value>> order = backward(T);
    std::cout << std::endl;
    // for(int i = 0; i < order.size(); i++){
    //     std::cout << "Value(data= " << order[i]->data << " | grad= " << order[i]->grad << ") Leaf nodes: "<< order[i]->left->data <<", " << order[i]->right->data <<  std::endl; 
    // }
    // Value* ptr = std::make_shared<Value>(c);
    // std::cout << ptr->data << std::endl;
    return 0;

}