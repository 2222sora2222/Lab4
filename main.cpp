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
    const E properties_; // Свойства ребра (например, вес).
    Vertex<V, E>* vertex1_; // Первая вершина, с которой связано ребро.
    Vertex<V, E>* vertex2_; // Вторая вершина, с которой связано ребро.
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
    const V properties_; // Свойство вершины (например, имя).
    std::vector<Edge<V, E>*> edges_; // Список рёбер, исходящих из этой вершины.
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

// Функция для вывода графа в консоль
template <class V, class E>
void printGraph(const std::vector<Vertex<V, E>*>& vertices) {
    std::cout << "Graph representation:\n";
    for (const auto& vertex : vertices) {
        std::cout << "Vertex " << *vertex->getProperties() << ":\n";
        for (const auto& edge : *vertex->getEdges()) {
            const Vertex<V, E>* target = edge->getVertex1() == vertex ? edge->getVertex2() : edge->getVertex1();
            std::cout << "  -> " << *target->getProperties() << " [weight: " << *edge->getProperties() << "]\n";
        }
    }
    std::cout << "End of graph\n";
}

// Класс для поиска пути (без учёта стоимости)
template <class V, class E, class C = std::equal_to<V>>
class PathBuilder : public OneTimeVisitor<V, E> {
public:
    // Конструктор: инициализация значением (целью поиска) и количеством путей для поиска.
    PathBuilder(const V& value, size_t pathCount = std::numeric_limits<size_t>::max())
        : value_(value), pathCount_(pathCount), pathes_(new std::vector<std::vector<const Vertex<V, E>*>>()) {}

    // Метод посещения вершины: проверяем, достигли ли целевой вершины.
    bool visitVertex(const Vertex<V, E>* vertex) {
        if (!OneTimeVisitor<V, E>::visitVertex(vertex)) { // Если вершина уже посещена.
            return false;
        }
        if (C()(*vertex->getProperties(), value_)) { // Если вершина соответствует цели.
            pathes_->push_back(OneTimeVisitor<V, E>::getVisited()); // Сохраняем путь.
            OneTimeVisitor<V, E>::leaveVertex(vertex); // Покидаем вершину.
            return false; // Останавливаем дальнейший обход.
        }
        return true;
    }

    // Метод посещения ребра.
    bool visitEdge(const Edge<V, E>* edge) {
        if (!OneTimeVisitor<V, E>::visitEdge(edge)) { // Если нельзя пройти ребро.
            return false;
        }
        if (pathes_->size() < pathCount_) { // Если количество найденных путей меньше запрашиваемого.
            return true;
        }
        OneTimeVisitor<V, E>::leaveEdge(edge); // Покидаем ребро.
        return false;
    }

private:
    const V value_; // Цель поиска.
    const size_t pathCount_; // Количество путей, которые нужно найти.
    std::shared_ptr<std::vector<std::vector<const Vertex<V, E>*>>> pathes_; // Найденные пути.
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
    a1.addEdge(10, &b1); // Ребро от A1 к B1 с весом 10.
    b1.addEdge(20, &c1); // Ребро от B1 к C1 с весом 20.
    a1.addEdge(15, &b2); // Ребро от A1 к B2 с весом 15.
    c1.addOrderedEdge(30, &e1); // Ориентированное ребро от C1 к E1 с весом 30.
    e1.addOrderedEdge(25, &f1); // Ориентированное ребро от E1 к F1 с весом 25.
    b2.addEdge(40, &f1); // Ребро от B2 к F1 с весом 40.

    // Список всех вершин
    std::vector<Vertex<std::string, int>*> vertices = {&a1, &b1, &c1, &b2, &e1, &f1};

    // Вывод графа
    printGraph(vertices);

    // Поиск пути от A1 до F1
    PathBuilder<std::string, int> pathBuilder("F1");
    depthPass(&a1, &pathBuilder);

    std::cout << "Vin\n";
    return 0;
}
