#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <opencv2/core/core.hpp>
#include "vectortree.h"
#include "region.h"

void buildTree(VectorTree& vectortree, std::vector<Region>& regions);

#endif