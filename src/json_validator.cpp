#include "json_validator.h"
#include "exception.h"
#include "utility.h"	// q
#include <cassert>
#include <sstream>
#include <utility>	// swap
#include <vector>


namespace JSONValHelper {

// convert stack to vector
template<class T>
static std::vector<T> toVector(std::stack<T>& st) {
	std::vector<T> vec{st.size()};
	for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
		std::swap(*it, st.top());
		st.pop();
	}
	return vec;
}


class MyVisitor : public boost::static_visitor<std::string> {
public:
	std::string operator()(const std::size_t key) const {
		assert(key != std::numeric_limits<std::size_t>::max());	// key was never set, invalid
		return ("index " + std::to_string(key));
	}

	std::string operator()(const std::string& key) const {
		assert(!key.empty());
		return q(key);
	}
};


static std::string genPathItem(const JSONValidator::Path& p) {
	std::string str;
	str = boost::apply_visitor(MyVisitor(), p.key);
	return str;
}


template<class Iter>	// vector<Path> iterator
static std::string genPathStr(Iter beg, const Iter end) {
	std::stringstream ss;
	if (beg != end) {
		ss << JSONValHelper::genPathItem(*beg);
		for (auto it = beg+1; it != end; ++it) {
			ss << ", " << JSONValHelper::genPathItem(*it);
		}
	}
	else {
		assert(false);	// this should not have been called...
	}
	return ss.str();
}


// If only root, path is simply root, else actual path
template<class T>
static std::string genPathStr(std::stack<T>& stack) {
	std::string str{"(path "};
	if (stack.size() == 1) {
		str += "root";
	}
	else {
		std::vector<T> vecPath = toVector(stack);
		str += genPathStr(vecPath.cbegin() + 1, vecPath.cend());
	}
	str += ')';
	return str;
}

} // namespace JSONValHelper


JSONValidator::JSONValidator(const rapidjson::Document* root, BadData& e) : error(e) {
	assert(root != nullptr);
	// the bottom of stack should always be root
	stack.emplace();
	stack.top().value = root;
}


void JSONValidator::enter(const std::size_t i) {
	checkKey(i);
	const rapidjson::Value* const newValue = &((*stack.top().value)[i]);
	stack.emplace();
	stack.top().key = i;
	assert(stack.top().key.which() == 0);
	stack.top().value = newValue;
}


void JSONValidator::enter(const std::string& key) {
	checkKey(key);
	const rapidjson::Value* const newValue = &((*stack.top().value)[key.c_str()]);
	stack.emplace();
	stack.top().key = key;
	assert(stack.top().key.which() == 1);
	stack.top().value = newValue;
}


void JSONValidator::leave() {
	assert(stack.size() > 1);
	if (stack.size() <= 1)
		throw RuntimeError{"JSONValidator::leave", "stack too empty"};
	stack.pop();
}


// Note: always check, even if valueType already set
void JSONValidator::checkArray() {
	if (stack.top().value->IsArray()) {
		stack.top().valueType = DataType::ARRAY;
	}
	else {
		throwError("is not array");
	}
}


void JSONValidator::checkInt() {
	if (stack.top().value->IsInt()) {
		stack.top().valueType = DataType::NUMBER;
	}
	else {
		throwError("is not int");
	}
}


// check int greater than or equal to
void JSONValidator::checkIntGE(const int n) {
	const int val = getInt();
	if (val < n) {
		throwError("int is less than " + std::to_string(n));
	}
}


void JSONValidator::checkObject() {
	if (stack.top().value->IsObject()) {
		stack.top().valueType = DataType::OBJECT;
	}
	else {
		throwError("is not object");
	}
}


void JSONValidator::checkString() {
	if (stack.top().value->IsString()) {
		stack.top().valueType = DataType::STRING;
	}
	else {
		throwError("is not string");
	}
}


void JSONValidator::checkUint() {
	if (stack.top().value->IsUint()) {
		stack.top().valueType = DataType::NUMBER;
	}
	else {
		throwError("is not uint");
	}
}


void JSONValidator::checkKey(const std::size_t i) {
	checkArray();
	assert(stack.top().valueType == DataType::ARRAY);
	const std::size_t sz = getSize();
	if (i >= sz) {
		throwError("index " + std::to_string(i) + " does not exist");
	}
}


void JSONValidator::checkKey(const std::string& key) {
	checkObject();
	assert(stack.top().valueType == DataType::OBJECT);
	if (!stack.top().value->HasMember(key.c_str())) {
		throwError("key " + q(key) + " does not exist");
	}
}


void JSONValidator::checkSize(const std::size_t sz) {
	const std::size_t valSz = getSize();
	if (sz != valSz) {
		throwError("is not size " + std::to_string(sz));
	}
}


void JSONValidator::checkSizeMult(const std::size_t m) {
	assert(m != 0);
	const std::size_t curSz = getSize();
	if (curSz % m != 0) {
		throwError("size is not multiple of " + std::to_string(m));
	}
}


// returns true if key exists
bool JSONValidator::exists(const std::string& key) {
	checkObject();
	return stack.top().value->HasMember(key.c_str());
}


int JSONValidator::getInt() {
	checkInt();
	return stack.top().value->GetInt();
}


unsigned int JSONValidator::getUint() {
	checkUint();
	return stack.top().value->GetUint();
}


std::size_t JSONValidator::getSize() {
	std::size_t sz;
	if (stack.top().valueType == DataType::NONE) {
		if (stack.top().value->IsObject())
			stack.top().valueType = DataType::OBJECT;
		else if (stack.top().value->IsArray())
			stack.top().valueType = DataType::ARRAY;
	}
	switch (stack.top().valueType) {
	case DataType::OBJECT:
		sz = stack.top().value->MemberCount();
		break;
	case DataType::ARRAY:
		sz = stack.top().value->Size();
		break;
	default:
		throwError("is not container type, cannot get size");
	}
	return sz;
}


std::string JSONValidator::getString() {
	checkString();
	return stack.top().value->GetString();
}


void JSONValidator::throwError(const std::string& append) {
	error.setDetails(JSONValHelper::genPathStr(stack) + ' ' + append);
	throw error;
}
