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
            // std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") " << res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }
        // Subtraction
        std::shared_ptr<Value> operator-(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(this->data - other->data);
            res->op = "-";
            res->left = shared_from_this();
            res->right = other;
            // std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") " << res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }
        // Multiply
        std::shared_ptr<Value> operator*(std::shared_ptr<Value> const& other){
            auto res = std::make_shared<Value>(other->data * this->data);
            res->op = "*";
            res->left = shared_from_this();
            res->right = other;
            // std::cout << "Value(data= " << this->data << " | grad= " << this->grad << ") "<< res->op << " Value(data= " << other->data << " | grad= " << other->grad << ")" << std::endl; 
            return res;
        }
        // Exponent
        std::shared_ptr<Value> power(const std::shared_ptr<Value>& base, float power){
            auto res = std::make_shared<Value>( std::pow(base->data, power) );
            res->op = "^";
            res->left = base;
            res->right = std::make_shared<Value>(power);
            // std::cout << "Value(data= " << base->data << " | grad= " << base->grad << ") "<< res->op << " float(data= " << power << ")" << std::endl; 
            return res;
        }
        // ReLU
        std::shared_ptr<Value> relu(const std::shared_ptr<Value>& in){
            auto out = std::make_shared<Value>(std::max(0.f, in->data));
            out->op = "relu";
            out->left = in;
            out->right = nullptr;
            // std::cout << "Value(data= " << out->op << "( " << in->data << " )= " << out->data << " | grad= " << out->grad << ") "<< std::endl; 
            return out;
        }
        // tanh
        std::shared_ptr<Value> tanh(const std::shared_ptr<Value>& in) {
            float t = std::tanh(in->data);
            auto out = std::make_shared<Value>(t);
            out->op = "tanh";
            out->left = in;
            out->right = nullptr;
            // std::cout << "Value(data= tanh(" << in->data << ") = " << out->data << " | grad= " << out->grad << ")\n";
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
        else if( v->op == "-"){
            if (v->left)  v->left->grad  += 1.0f * v->grad;
            if (v->right) v->right->grad -= 1.0f * v->grad;
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
        else if( v->op == "tanh" ){
            if (v->left) {
                float t = std::tanh(v->left->data);
                v->left->grad += (1 - t * t) * v->grad;
            }
        }
        // std::cout << "Value(data= " << v->data << " | grad= " << v->grad << ") "<< std::endl;    
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

// Subtraction
std::shared_ptr<Value> operator-(
    const std::shared_ptr<Value>& lhs,
    const std::shared_ptr<Value>& rhs) {
    return lhs->operator-(rhs);
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

std::shared_ptr<Value> tanh(const std::shared_ptr<Value>& in) {
    return in->tanh(in);
}

class Neuron{
    public:
        std::shared_ptr<Value> b;
        std::vector<std::shared_ptr<Value>> w; 
        float w_init = 1;
    
        Neuron(int nin){ //where nin is dimension of the input eg. dim of x in y = f(w.x + b)
            for( int i = 0; i < nin ; i++){
                w.push_back(std::make_shared<Value>((float)(rand() % 200 - 100) / 100));
            }
            b = std::make_shared<Value>(0.0f);
        }

        std::shared_ptr<Value> forward(std::vector<std::shared_ptr<Value>>& x){ // y = f(w.x + b)
            auto act = b;
            for(int i = 0; i < w.size(); i++){
                act = act + (x[i] * w[i]);   
            }
            auto out = tanh(act);
            return out;
        }

        std::vector<std::shared_ptr<Value>> parameters(){
            std::vector<std::shared_ptr<Value>> params = w;
            params.push_back(b);
            return params;
        }


};

class Layer{
    public:
        std::vector<std::shared_ptr<Neuron>> neurons;
        
        Layer(int nin, int nout){
            for(int i = 0; i < nout; i++){
                neurons.push_back(std::make_shared<Neuron>(nin));
            }
        }

        std::vector<std::shared_ptr<Value>> forward(std::vector<std::shared_ptr<Value>>& x){
            std::vector<std::shared_ptr<Value>> outs;
            for(int i = 0; i < neurons.size(); i++){
                outs.push_back( neurons[i]->forward(x) );
            }
            return outs;
        }

        std::vector<std::shared_ptr<Value>> parameters() {
            std::vector<std::shared_ptr<Value>> params;
            for (auto& neuron : neurons) {
                auto p = neuron->parameters();
                params.insert(params.end(), p.begin(), p.end());
            }
            return params;
        }
};

class MLP{
    public:
        std::vector<std::shared_ptr<Layer>> layers;
        
        MLP(int nin, std::vector<int> nouts){

            std::vector<int> sz;
            sz.push_back(nin);
            sz.insert(sz.end(), nouts.begin(), nouts.end());

            for(int i = 0; i < nouts.size(); i++){
                layers.push_back(std::make_shared<Layer>(sz[i],sz[i+1]));
            }
        }

        std::vector<std::shared_ptr<Value>> forward(std::vector<std::shared_ptr<Value>> &x){
            std::vector<std::shared_ptr<Value>> out = x;
            for (int i = 0; i < layers.size(); i++){
                out = layers[i]->forward(out) ;
            }
            return out;
        }

        std::vector<std::shared_ptr<Value>> parameters() {
            std::vector<std::shared_ptr<Value>> params;
            for (auto& layer : layers) {
                auto p = layer->parameters();
                params.insert(params.end(), p.begin(), p.end());
            }
            return params;
        }
};

int main() {
     // Assemble MLP
    std::vector<int> layersz = {4, 4, 1};
    auto n = std::make_shared<MLP>(3, layersz); // {3(i), 4, 4, 1(o)}
    
    // Raw Training Data
    std::vector<std::vector<float>> inputs = { {2 , 3, -1}, {3, -1, 0.5}, {0.5, 1, 1}, {1, 1, -1}};
    std::vector<float> outputs = {1, -1, -1, 1};
   
    // Format data into the tractable types
    std::vector<std::shared_ptr<Value>> ys; 
    for(int i = 0; i < outputs.size(); i++){
        ys.push_back(std::make_shared<Value>(outputs[i]));
    }

    std::vector<std::vector<std::shared_ptr<Value>>> xs;
    for(int i = 0; i < inputs.size(); i++){
        std::vector<std::shared_ptr<Value>> x;
        for( int j = 0; j < inputs[i].size(); j++){
            x.push_back(std::make_shared<Value>(inputs[i][j]));
        }
        xs.push_back(x);
    }

    // initial forward pass
    std::vector<std::shared_ptr<Value>> ypred;

    for(int i = 0; i < xs.size(); i++){
        auto y = n->forward(xs[i]);
        ypred.push_back(y[0]); // Note the y[0] assumes the final layer is of size 1
    }

    // Training loop
    for(int step = 0; step < 10; step++){
        // Forward pass - output
        for(int i = 0; i < xs.size(); i++){
            auto y = n->forward(xs[i]);
            ypred[i] = (y[0]); // Again y[0], final layer of size 1 assumption
        }

        std::cout << "Step: " << step << std::endl;
        std::cout << "Value(data= ";
        for(int i = 0; i < ypred.size(); i++){
            std::cout  << ypred[i]->data << ", ";
        }
        std::cout << ") " << std::endl;
        // backward pass - loss
        auto loss = std::make_shared<Value>(0);
        for(int i = 0; i < ys.size(); i++){
            loss = loss + power(ys[i] - ypred[i], 2.0f);
        }

        std::cout<< "Loss= " << loss << std::endl;
        std::vector<std::shared_ptr<Value>> order = backward(loss);

        // gradient descent
        float learning_rate = 0.01;
        for (auto& layer : n->layers) {
            for (auto& neuron : layer->neurons) {
                for (auto& p : neuron->parameters()) {
                    p->data -= learning_rate * p->grad;
                    p->grad = 0.0f; // reset grad for next iteration
                }
            }
        }

    }



    std::cout << "total params= " << n->parameters().size() << std::endl;
    // for(int i = 0; i < out.size(); i++){
    //     std::cout << "Value(data= " << out[i]->data << " | grad= " << out[i]->grad << ") " <<  std::endl; 
    // }
    return 0;

}