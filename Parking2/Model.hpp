#pragma once
template <class type> struct Model {
    type v;
};

template <class type> bool operator< (const Model<type>& model1, const Model<type>& model2) {
    return model1.v < model2.v;
}

template <class type> bool operator== (const Model<type>& model1, const Model<type>& model2) {
    return model1.v == model2.v;
}

template <class type> bool operator!= (const Model<type>& model1, const Model<type>& model2) {
    return model1.v != model2.v;
}

template <class type> bool operator<= (const Model<type>& model1, const Model<type>& model2) {
    return model1 == model2 or model1 < model2;
}
