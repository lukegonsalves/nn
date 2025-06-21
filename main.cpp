#include <Eigen/Dense>
#include <iostream>
#include <memory>
#include <stack>

class Value {
    public: 
        float data, grad;
        std::shared_ptr<Value> left = nullptr , right = nullptr;
        std::string op;

        Value() : data(0), grad(0) {}
        Value(float x) : data(x), grad(0), op("") {}

        // Arithmetic Operators
        Value operator+(Value const& other){
            auto res = std::make_shared<Value>(other.data + data);
            res->op = "+";
            res->left = std::make_shared<Value>(*this);
            res->right = std::make_shared<Value>(other);
            std::cout << "Value(data= " << data << " | grad= " << grad << ") " << res->op << " Value(data= " << other.data << " | grad= " << other.grad << ")" << std::endl; 
            return *res;
        }

        Value operator*(Value const& other){
            auto res = std::make_shared<Value>(other.data * data);
            res->op = "*";
            res->left = std::make_shared<Value>(*this);
            res->right = std::make_shared<Value>(other);
            std::cout << "Value(data= " << data << " | grad= " << grad << ") "<< res->op << " Value(data= " << other.data << " | grad= " << other.grad << ")" << std::endl; 
            return *res;
        }

        // ostream
        friend std::ostream& operator<<(std::ostream& out, const Value& obj){
            out << "Value(data= " << obj.data << " | grad= " << obj.grad << ")" << std::endl;
            return out;
        }
};


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

 

    Value a(2), b(-3), c(10), f(-2);
    Value e = a*b;
    Value d = e + c;
    Value L = d * f;
    Value exp = L * 1;
    // L = ((a2*b-3)+c10) * f-2
    std::cout << L << std::endl;

    std::stack<std::shared_ptr<Value>> s;
    std::shared_ptr<Value> head = exp.left;
    s.push(head);
    while ( !s.empty() ){
        std::shared_ptr<Value> temp = s.top();
        std::cout << s.top()->data << std::endl;    
        s.pop();
        if( temp->left != NULL ){
            s.push(temp->left);
        }
        if( temp->right != NULL ){
            s.push(temp->right);
        }
    }
    // Value* ptr = std::make_shared<Value>(c);
    // std::cout << ptr->data << std::endl;
    return 0;

}