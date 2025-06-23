#include <Eigen/Dense>
#include <iostream>
#include <memory>
#include <stack>

class Value {
    public: 
        float data, grad;
        std::shared_ptr<Value> left = nullptr , right = nullptr;
        std::string op;

        Value() : data(0), grad(1) {}
        Value(float x) : data(x), grad(1), op("") {}

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
    Value e = a*b; // de/de = 1, de/da = b, de/db == a;
    Value d = e + c; // dd/dd = 1, dd/de = c, dd/dc = e;
    Value L = d * f; // dL/dd = f, dL/df = d;
    Value exp = L * 1;
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

    std::cout << L << std::endl;

    std::stack<std::shared_ptr<Value>> s;
    std::shared_ptr<Value> head = exp.left;
    s.push(head);
    while ( !s.empty() ){
        std::shared_ptr<Value> temp = s.top();
        std::cout << "Value(data= " << temp->data << " | grad= " << temp->grad << ") "<< std::endl;    
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