/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "wm_occlusion_region.h"

#include <map>
#include <set>

namespace OHOS::Rosen::WmOcclusion {
static Rect _s_empty_rect_ { 0, 0, 0, 0 };
static Rect _s_invalid_rect_ { 0, 0, -1, -1 };
bool Region::_s_so_loaded_ = false;

std::ostream& operator<<(std::ostream& os, const Rect& r)
{
    os << "{" << r.left_ << "," << r.top_ << "," << r.right_ << "," << r.bottom_ << "}";
    return os;
}

void Node::Update(int updateStart, int updateEnd, Event::Type type)
{
    if (updateStart >= updateEnd) {
        return;
    }
    if (updateStart == start_ && updateEnd == end_) {
        if (type == Event::Type::CLOSE || type == Event::Type::OPEN) {
            positive_count_ += type;
        } else {
            negative_count_ += type;
        }
    } else {
        if (right_ == nullptr) {
            right_ = new Node { mid_, end_ };
        }
        if (left_ == nullptr) {
            left_ = new Node { start_, mid_ };
        }
        left_->Update(updateStart, mid_ < updateEnd ? mid_ : updateEnd, type);
        right_->Update(mid_ > updateStart ? mid_ : updateStart, updateEnd, type);
    }
}

bool EventSortByY(const Event& e1, const Event& e2)
{
    if (e1.y_ == e2.y_) {
        return e1.type_ < e2.type_;
    }
    return e1.y_ < e2.y_;
}

void Node::GetOrRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    bool isPos = isParentNodePos || (positive_count_ > 0);

    if (isNeg || isPos) {
        PushRange(res);
    } else {
        if (left_ != nullptr) {
            left_->GetOrRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetOrRange(res, isPos, isNeg);
        }
    }
}

void Node::GetAndRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    bool isPos = isParentNodePos || (positive_count_ > 0);

    if (isNeg && isPos) {
        PushRange(res);
    } else {
        if (left_ != nullptr) {
            left_->GetAndRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetAndRange(res, isPos, isNeg);
        }
    }
}

void Node::GetSubRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    bool isPos = isParentNodePos || (positive_count_ > 0);

    if (isPos && !isNeg && IsLeaf()) {
        PushRange(res);
    } else if (isNeg) {
        return;
    } else {
        if (left_ != nullptr) {
            left_->GetSubRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetSubRange(res, isPos, isNeg);
        }
    }
}

void Node::GetXOrRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    bool isPos = isParentNodePos || (positive_count_ > 0);

    if (((isPos && !isNeg) || (!isPos && isNeg)) && IsLeaf()) {
        PushRange(res);
    } else if (isNeg && isPos) {
        return;
    } else {
        if (left_ != nullptr) {
            left_->GetXOrRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetXOrRange(res, isPos, isNeg);
        }
    }
}

void Region::getRange(std::vector<Range>& ranges, Node& node, Region::OP op)
{
    switch (op) {
        case Region::OP::AND:
            node.GetAndRange(ranges);
            break;
        case Region::OP::SUB:
            node.GetSubRange(ranges);
            break;
        case Region::OP::OR:
            node.GetOrRange(ranges);
            break;
        case Region::OP::XOR:
            node.GetXOrRange(ranges);
            break;
        default:
            break;
    }
    return;
}

void MakeEnumerate(std::set<int>& ys, std::map<int, int>& indexOf, std::vector<int>& indexAt)
{
    int index = 0;
    auto it = ys.begin();
    while (it != ys.end()) {
        indexOf[*it] = index++;
        indexAt.push_back(*it);
        ++it;
    }
    return;
}

void Region::UpdateRects(Rects& r, std::vector<Range>& ranges, std::vector<int>& indexAt, Region& res)
{
    uint32_t i = 0;
    uint32_t j = 0;
    while (i < r.preRects.size() && j < ranges.size()) {
        if (r.preRects[i].left_ == indexAt[ranges[j].start_] && r.preRects[i].right_ == indexAt[ranges[j].end_]) {
            r.curRects.emplace_back(Rect { r.preRects[i].left_, r.preRects[i].top_, r.preRects[i].right_, r.curY });
            j++;
            i++;
        } else if (r.preRects[i].right_ < indexAt[ranges[j].end_]) {
            res.GetRegionRects().push_back(r.preRects[i]);
            i++;
        } else {
            r.curRects.emplace_back(Rect { indexAt[ranges[j].start_], r.preY, indexAt[ranges[j].end_], r.curY });
            j++;
        }
    }

    for (; j < ranges.size(); j++) {
        r.curRects.emplace_back(Rect { indexAt[ranges[j].start_], r.preY, indexAt[ranges[j].end_], r.curY });
    }
    for (; i < r.preRects.size(); i++) {
        res.GetRegionRects().push_back(r.preRects[i]);
    }
    r.preRects.clear();
    r.preRects.swap(r.curRects);
    return;
}

void Region::MakeBound()
{
    if (rects_.size()) {
        bound_ = rects_[0];
        for (const auto& r : rects_) {
            bound_.top_ = std::min(r.top_, bound_.top_);
            bound_.left_ = std::min(r.left_, bound_.left_);
            bound_.right_ = std::max(r.right_, bound_.right_);
            bound_.bottom_ = std::max(r.bottom_, bound_.bottom_);
        }
    }
}

void Region::RegionOpLocal(Region& r1, Region& r2, Region& res, Region::OP op)
{
    r1.MakeBound();
    r2.MakeBound();
    res.GetRegionRects().clear();
    std::set<int> xs;
    std::vector<Event> events;

    for (auto& rect : r1.GetRegionRects()) {
        events.emplace_back(Event { rect.top_, Event::Type::OPEN, rect.left_, rect.right_ });
        events.emplace_back(Event { rect.bottom_, Event::Type::CLOSE, rect.left_, rect.right_ });
        xs.insert(rect.left_);
        xs.insert(rect.right_);
    }
    for (auto& rect : r2.GetRegionRects()) {
        events.emplace_back(Event { rect.top_, Event::Type::VOID_OPEN, rect.left_, rect.right_ });
        events.emplace_back(Event { rect.bottom_, Event::Type::VOID_CLOSE, rect.left_, rect.right_ });
        xs.insert(rect.left_);
        xs.insert(rect.right_);
    }

    if (events.empty()) {
        return;
    }

    std::vector<int> indexAt;
    std::map<int, int> indexOf;
    MakeEnumerate(xs, indexOf, indexAt);
    sort(events.begin(), events.end(), EventSortByY);
    size_t indexOfSize = indexOf.size() > 0 ? (indexOf.size() - 1) : 0;
    Node rootNode { 0, static_cast<int>(indexOfSize) };

    std::vector<Range> ranges;
    Rects r;
    r.curY = events[0].y_;
    r.preY = events[0].y_;
    for (auto& event : events) {
        r.curY = event.y_;
        ranges.clear();
        getRange(ranges, rootNode, op);
        if (r.curY > r.preY) {
            UpdateRects(r, ranges, indexAt, res);
        }
        rootNode.Update(indexOf[event.left_], indexOf[event.right_], event.type_);
        r.preY = r.curY;
    }
    copy(r.preRects.begin(), r.preRects.end(), back_inserter(res.GetRegionRects()));
    res.MakeBound();
}

void Region::RegionOp(Region& r1, Region& r2, Region& res, Region::OP op)
{
    RegionOpLocal(r1, r2, res, op);
}

Region& Region::OperationSelf(Region& r, Region::OP op)
{
    Region r1(*this);
    RegionOp(r1, r, *this, op);
    return *this;
}

Region Region::Or(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::OR);
    return res;
}

Region Region::And(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::AND);
    return res;
}

Region Region::Sub(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::SUB);
    return res;
}

Region Region::Xor(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::XOR);
    return res;
}

Region& Region::OrSelf(Region& r)
{
    return OperationSelf(r, Region::OP::OR);
}

Region& Region::AndSelf(Region& r)
{
    return OperationSelf(r, Region::OP::AND);
}

Region& Region::SubSelf(Region& r)
{
    return OperationSelf(r, Region::OP::SUB);
}

Region& Region::XOrSelf(Region& r)
{
    return OperationSelf(r, Region::OP::XOR);
}

std::ostream& operator<<(std::ostream& os, const Region& r)
{
    os << "{";
    os << r.GetSize() << ": ";
    for (const Rect& regionRect : r.GetRegionRects()) {
        os << regionRect;
    }
    os << "}" << std::endl;
    return os;
}
} // namespace OHOS