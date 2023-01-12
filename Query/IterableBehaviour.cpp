//
// Created by Walker on 2022/9/16.
//
#include "Distinct.h"
#include "Group.h"
#include "Intersection.h"
#include "Join.h"
#include "Projection.h"
#include "Select.h"
#include "Sort.h"
#include "Union.h"
#include "Sub.h"

Iterator*     DistinctQuery::GenerateIterator() {
    return new DistinctQueryIterator(*this);
}
Iterator*        GroupQuery::GenerateIterator() {
    return new GroupQueryIterator(*this);
}
Iterator* IntersectionQuery::GenerateIterator() {
    return new IntersectionQueryIterator(*this);
}
Iterator*         JoinQuery::GenerateIterator() {
    return new JoinQueryIterator(*this);
}
Iterator*   ProjectionQuery::GenerateIterator() {
    return new ProjectionQueryIterator(*this);
}
Iterator*       SelectQuery::GenerateIterator() {
    return new SelectQueryIterator(*this);
}
Iterator*         SortQuery::GenerateIterator() {
    return new SortQueryIterator(*this);
}
Iterator*          SubQuery::GenerateIterator() {
    return new SubQueryIterator(*this);
}
Iterator*        UnionQuery::GenerateIterator() {
    return new UnionQueryIterator(*this);
}



FileSystem &DistinctQuery::FS() {
    return iterable.FS();
}
FileSystem &GroupQuery::FS() {
    return iterable.FS();
}
FileSystem &IntersectionQuery::FS() {
    return a.FS();
}
FileSystem &JoinQuery::FS() {
    return a.FS();
}
FileSystem &ProjectionQuery::FS() {
    return iterable.FS();
}
FileSystem &SelectQuery::FS() {
    return iterable.FS();
}
FileSystem &SortQuery::FS() {
    return iterable.FS();
}
FileSystem &SubQuery::FS() {
    return a.FS();
}
FileSystem &UnionQuery::FS() {
    return a.FS();
}


Size_t DistinctQuery::RecordSize() {
    return iterable.RecordSize();
}
Size_t GroupQuery::RecordSize() {
    return iterable.RecordSize();
}
Size_t IntersectionQuery::RecordSize() {
    if (a.RecordSize() != b.RecordSize())
        throw 234;
    else {
        return a.RecordSize();
    }
}
Size_t JoinQuery::RecordSize() {
    if (ap.Size() != bp.Size())
        throw 234;
    else {
        return a.RecordSize() + b.RecordSize();
    }
}
Size_t ProjectionQuery::RecordSize() {
    Int sum = 0;
    for (Int i = 0; i < part.index_num; i++) {
        sum += part.range[i].second;
    } return sum;
}
Size_t SelectQuery::RecordSize() {
    return iterable.RecordSize();
}
Size_t SortQuery::RecordSize() {
    return iterable.RecordSize();
}
Size_t SubQuery::RecordSize() {
    if (a.RecordSize() != b.RecordSize())
        throw 234;
    else {
        return a.RecordSize();
    }
}
Size_t UnionQuery::RecordSize() {
    if (a.RecordSize() != b.RecordSize())
        throw ERROR_RECORD_SIZE;
    else {
        return a.RecordSize();
    }
}