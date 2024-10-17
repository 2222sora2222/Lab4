#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <memory>
#include <limits>

// Предварительное объявление класса Vertex, чтобы его можно было использовать в Edge
template <class V, class E>
class Vertex;

// Класс ребра
template <class V, class E>
class Edge {
public:
    Edge(const E& properties, Vertex<V, E>* vertex1, Vertex<V, E>* vertex2)
        : properties_(properties), vertex1_(vertex1), vertex2_(vertex2) {}

    const Vertex<V, E>* getVertex1() const { return vertex1_; }
    const Vertex<V, E>* getVertex2() const { return vertex2_; }
    const E* getProperties() const { return &properties_; }

private:
    const E properties_;
    Vertex<V, E>* vertex1_;
    Vertex<V, E>* vertex2_;
};

// Класс вершины
template <class V, class E>
class Vertex {
public:
    Vertex(const V& properties) : properties_(properties) {}

    const V* getProperties() const { return &properties_; }
    const std::vector<Edge<V, E>*>* getEdges() const { return &edges_; }

    void addOrderedEdge(const E& properties, Vertex<V, E>* target) {
        Edge<V, E>* edge = new Edge<V, E>(properties, target, nullptr);
        edges_.push_back(edge);
    }

    void addEdge(const E& properties, Vertex<V, E>* target) {
        Edge<V, E>* edge = new Edge<V, E>(properties, target, this);
        edges_.push_back(edge);
        target->edges_.push_back(edge);
    }

private:
    const V properties_;
    std::vector<Edge<V, E>*> edges_;
};

// Класс для посетителя (Visitor) для обхода графа
template <class V, class E>
class OneTimeVisitor {
public:
    bool visitVertex(const Vertex<V, E>* vertex) {
        if (std::find(visited_.begin(), visited_.end(), vertex) != visited_.end()) {
            return false;
        }
        visited_.push_back(vertex);
        return true;
    }

    bool visitEdge(const Edge<V, E>*) {
        return true;
    }

    void leaveVertex(const Vertex<V, E>*) { visited_.pop_back(); }
    void leaveEdge(const Edge<V, E>*) {}

    const std::vector<const Vertex<V, E>*>& getVisited() const { return visited_; }

private:
    std::vector<const Vertex<V, E>*> visited_;
};

// Шаблон обхода графа в глубину
template <class V, class E, class F>
void depthPass(const Vertex<V, E>* vertex, F* visitor) {
    if (!visitor->visitVertex(vertex)) {
        return;
    }
    for (Edge<V, E>* edge : *vertex->getEdges()) {
        if (!visitor->visitEdge(edge)) {
            continue;
        }

        const Vertex<V, E>* next = edge->getVertex1() == vertex || edge->getVertex1() == nullptr ? edge->getVertex2() : edge->getVertex1();
        depthPass(next, visitor);
        visitor->leaveEdge(edge);
    }
    visitor->leaveVertex(vertex);
}

// Класс для поиска пути (без учета стоимости)
template <class V, class E, class C = std::equal_to<V>>
class PathBuilder : public OneTimeVisitor<V, E> {
public:
    PathBuilder(const V& value, size_t pathCount = std::numeric_limits<size_t>::max())
        : value_(value), pathCount_(pathCount), pathes_(new std::vector<std::vector<const Vertex<V, E>*>>()) {}

    bool visitVertex(const Vertex<V, E>* vertex) {
        if (!OneTimeVisitor<V, E>::visitVertex(vertex)) {
            return false;
        }
        if (C()(*vertex->getProperties(), value_)) {
            pathes_->push_back(OneTimeVisitor<V, E>::getVisited());
            OneTimeVisitor<V, E>::leaveVertex(vertex);
            return false;
        }
        return true;
    }

    bool visitEdge(const Edge<V, E>* edge) {
        if (!OneTimeVisitor<V, E>::visitEdge(edge)) {
            return false;
        }
        if (pathes_->size() < pathCount_) {
            return true;
        }
        OneTimeVisitor<V, E>::leaveEdge(edge);
        return false;
    }

private:
    const V value_;
    const size_t pathCount_;
    std::shared_ptr<std::vector<std::vector<const Vertex<V, E>*>>> pathes_;
};

// Пример использования
int main() {
    // Создание вершин
    Vertex<std::string, int> a1("A1");
    Vertex<std::string, int> b1("B1");
    Vertex<std::string, int> c1("C1");
    Vertex<std::string, int> b2("B2");
    Vertex<std::string, int> e1("E1");
    Vertex<std::string, int> f1("F1");

    // Создание рёбер
    a1.addEdge(10, &b1);
    b1.addEdge(20, &c1);
    a1.addEdge(15, &b2);
    c1.addOrderedEdge(30, &e1);
    e1.addOrderedEdge(25, &f1);
    b2.addEdge(40, &f1);

    // Поиск пути от A1 до F1
    PathBuilder<std::string, int> pathBuilder("F1");
    depthPass(&a1, &pathBuilder);
   std::cout<< "Vin";
    return 0;
}
