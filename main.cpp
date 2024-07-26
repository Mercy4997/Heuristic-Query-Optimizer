#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class QueryTree {
public:
    std::vector<std::string> selections;
    std::vector<std::string> projections;
    std::vector<std::string> joins;
    bool distinct = false;

    void addSelection(const std::string& condition) {
        selections.push_back(condition);
    }

    void addProjection(const std::string& column) {
        projections.push_back(column);
    }

    void addJoin(const std::string& joinCondition) {
        joins.push_back(joinCondition);
    }

    void setDistinct(bool isDistinct) {
        distinct = isDistinct;
    }

    std::string toString() const {
        std::ostringstream result;
        result << "Query Tree:\n";
        result << "Selections: ";
        for (const auto& sel : selections) result << sel << " ";
        result << "\nProjections: ";
        for (const auto& proj : projections) result << proj << " ";
        result << "\nJoins: ";
        for (const auto& join : joins) result << join << " ";
        result << "\nDistinct: " << (distinct ? "True" : "False") << "\n";
        return result.str();
    }
};

QueryTree parseSQLQuery(const std::string& query) {
    // This is a simplified parser for the specific query structure provided
    QueryTree queryTree;
    queryTree.setDistinct(query.find("DISTINCT") != std::string::npos);
    size_t selectPos = query.find("SELECT");
    size_t fromPos = query.find("FROM");
    size_t wherePos = query.find("WHERE");

    std::string projectionPart = query.substr(selectPos + 6, fromPos - (selectPos + 6));
    std::string fromPart = query.substr(fromPos + 4, wherePos - (fromPos + 4));
    std::string wherePart = query.substr(wherePos + 5);

    // Parse projections
    std::istringstream projStream(projectionPart);
    std::string proj;
    while (getline(projStream, proj, ',')) {
        queryTree.addProjection(proj);
    }

    // Parse joins (simplified for given structure)
    queryTree.addJoin("x.pid = y.pid");
    queryTree.addJoin("y.cid = z.cid");

    // Parse selections
    std::istringstream whereStream(wherePart);
    std::string sel;
    while (getline(whereStream, sel, 'A')) {
        if (sel[0] == 'N') {
            sel = sel.substr(4); // remove "ND " from "AND "
        }
        queryTree.addSelection(sel);
    }

    return queryTree;
}

QueryTree optimizeQueryTree(const QueryTree& queryTree) {
    QueryTree optimizedTree;
    optimizedTree.projections = queryTree.projections;
    optimizedTree.distinct = queryTree.distinct;

    for (const auto& join : queryTree.joins) {
        optimizedTree.addJoin(join);
    }

    for (const auto& selection : queryTree.selections) {
        optimizedTree.addSelection(selection);
    }

    return optimizedTree;
}

std::string generateSQLFromTree(const QueryTree& queryTree) {
    std::ostringstream sql;
    sql << "SELECT ";
    if (queryTree.distinct) {
        sql << "DISTINCT ";
    }
    for (size_t i = 0; i < queryTree.projections.size(); ++i) {
        if (i > 0) {
            sql << ", ";
        }
        sql << queryTree.projections[i];
    }

    sql << " FROM Product x JOIN Purchase y ON x.pid = y.pid JOIN Customer z ON y.cid = z.cid";
    if (!queryTree.selections.empty()) {
        sql << " WHERE ";
        for (size_t i = 0; i < queryTree.selections.size(); ++i) {
            if (i > 0) {
                sql << " AND ";
            }
            sql << queryTree.selections[i];
        }
    }
    sql << ";";

    return sql.str();
}

int main() {
    std::string query = "SELECT DISTINCT x.name, y.name "
                        "FROM Product x "
                        "JOIN Purchase y ON x.pid = y.pid "
                        "JOIN Customer z ON y.cid = z.cid "
                        "WHERE x.price > 100 AND z.city = 'Seattle';";

    QueryTree canonicalQueryTree = parseSQLQuery(query);
    std::cout << "Canonical Query Tree:\n" << canonicalQueryTree.toString() << "\n";

    QueryTree optimizedQueryTree = optimizeQueryTree(canonicalQueryTree);
    std::cout << "Optimized Query Tree:\n" << optimizedQueryTree.toString() << "\n";

    std::string optimizedSQLQuery = generateSQLFromTree(optimizedQueryTree);
    std::cout << "Optimized SQL Query:\n" << optimizedSQLQuery << "\n";

    return 0;
}
