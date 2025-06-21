#include <Eigen/Dense>
#include <iostream>
#include <memory>

class Value {
    public: 
        float data;
        std::shared_ptr<Value> left = nullptr , right = nullptr;
        std::string op;

        Value() : data(0) {}
        Value(float x) : data(x), op("") {}

        // Arithmetic Operators
        Value operator+(Value const& other){
            auto res = std::make_shared<Value>(other.data + data);
            res->op = "+";
            res->left = std::make_shared<Value>(*this);
            res->right = std::make_shared<Value>(other);
            std::cout << data << " " << res->op << " " << other.data << std::endl; 
            return *res;
        }

        Value operator*(Value const& other){
            auto res = std::make_shared<Value>(other.data * data);
            res->op = "*";
            res->left = std::make_shared<Value>(*this);
            res->right = std::make_shared<Value>(other);
            std::cout << data << " " << res->op << " " << other.data << std::endl; 
            return *res;
        }

        // ostream
        friend std::ostream& operator<<(std::ostream& out, const Value& obj){
            out << "Value(data=" << obj.data << " | " << obj.left.get() << " " << obj.right.get() <<")" << std::endl;
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

    Value a(2), b(-3), c(10);
    Value d = a * b + c;
    std::cout << d << std::endl;
    // Value* ptr = std::make_shared<Value>(c);
    // std::cout << ptr->data << std::endl;
    return 0;

}