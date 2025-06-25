#include <Eigen/Dense>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <math.h>

class Value : public std::enable_shared_from_this<Value>{
    public: 
        float data, grad, back;
        std::shared_ptr<Value> left, right;
        std::string op;

        Value() : data(0), grad(0) {}
        Value(float x) : data(x), grad(0), op("") {}

        // Arithmetic Operators
        // Addition
        std::shared_ptr<Value> operator+(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(other->data + this->data);
            res->op = "+";
            res->left = shared_from_this();
            res->right = other;
            std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") " << res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }
        // Multiply
        std::shared_ptr<Value> operator*(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(other->data * this->data);
            res->op = "*";
            res->left = shared_from_this();
            res->right = other;
            std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") "<< res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }
        // Exponent
        std::shared_ptr<Value> power(const std::shared_ptr<Value>& base, float power){
            auto res = std::make_shared<Value>( std::pow(base->data, power) );
            res->op = "^";
            res->left = base;
            res->right = std::make_shared<Value>(power);
            std::cout << "Value(data= " << base->data << " | grad= " << base->grad << ") "<< res->op << " float(data= " << power << ")" << std::endl; 
            return res;
        }
        // ReLU
        std::shared_ptr<Value> relu(const std::shared_ptr<Value>& in){
            auto out = std::make_shared<Value>(std::max(0.f, in->data));
            out->op = "relu";
            out->left = in;
            out->right = nullptr;
            std::cout << "Value(data= " << out->op << "( " << in->data << " )= " << out->data << " | grad= " << out->grad << ") "<< std::endl; 
            return out;
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
        else if( v->op == "^"){
            if ( v->left ) v->right->grad = 0;
            if ( v->right ) v->left->grad += v->right->data * std::pow(v->left->data, v->right->data - 1) *v->grad;
        }
        else if( v->op == "relu"){
            if( v->left ) v->left->grad = 1 * v->grad;
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

std::shared_ptr<Value> operator*(const std::shared_ptr<Value>& lhs, float rhs) {
    return lhs->operator*(std::make_shared<Value>(rhs));
}

std::shared_ptr<Value> operator*(float lhs, const std::shared_ptr<Value>& rhs) {
    return std::make_shared<Value>(lhs)->operator*(rhs);
}

std::shared_ptr<Value> power(const std::shared_ptr<Value>& base, float power) {
    return base->power(base, power);
}

std::shared_ptr<Value> relu(const std::shared_ptr<Value>& in) {
    return in->relu(in);
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
    auto T = power(L, 1.0f);
    auto U = relu(T);
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

    std::vector<std::shared_ptr<Value>> order = backward(U);
    std::cout << std::endl;
    // for(int i = 0; i < order.size(); i++){
    //     std::cout << "Value(data= " << order[i]->data << " | grad= " << order[i]->grad << ") Leaf nodes: "<< order[i]->left->data <<", " << order[i]->right->data <<  std::endl; 
    // }
    // Value* ptr = std::make_shared<Value>(c);
    // std::cout << ptr->data << std::endl;
    return 0;

}