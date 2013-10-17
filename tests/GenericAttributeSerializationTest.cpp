/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericAttributeSerializationTest.h"
#include <cppunit/TestAssert.h>
#include <Util/GenericAttribute.h>
#include <Util/GenericAttributeSerialization.h>
#include <Util/StringIdentifier.h>
#include <functional>
#include <sstream>
#include <tuple>
#include <utility>
CPPUNIT_TEST_SUITE_REGISTRATION(GenericAttributeSerializationTest);

template<typename AttributeType, typename ValueType, typename ConversionFunction>
static void testGenericAttributeSerialization(ValueType value, const std::string & stringValue, const std::string & type, ConversionFunction toValue, const Util::GenericAttributeMap * context = nullptr) {
	std::unique_ptr<Util::GenericAttribute> genericAttribute(new AttributeType(value));
	auto content = Util::GenericAttributeSerialization::serialize(genericAttribute.get());
	CPPUNIT_ASSERT_EQUAL(std::string("{\"type\":\"") + type + std::string("\",\"value\":\"") + stringValue + std::string("\"}"), content);
	std::unique_ptr<Util::GenericAttribute> attribute(Util::GenericAttributeSerialization::unserialize(content, context));
	CPPUNIT_ASSERT(typeid(*genericAttribute.get()) == typeid(*attribute.get()));
	CPPUNIT_ASSERT_EQUAL(std::bind(toValue, genericAttribute.get())(), std::bind(toValue, attribute.get())());
}

void GenericAttributeSerializationTest::testStandardSerialization() {
	testGenericAttributeSerialization<Util::BoolAttribute>(true, "true", "bool", &Util::GenericAttribute::toBool);
	testGenericAttributeSerialization<Util::BoolAttribute>(false, "false", "bool", &Util::GenericAttribute::toBool);

	testGenericAttributeSerialization<Util::_NumberAttribute<double>>(25.6789, "25.6789", "numberDouble", &Util::GenericAttribute::toFloat);
	testGenericAttributeSerialization<Util::_NumberAttribute<float>>(12.345f, "12.345", "numberFloat", &Util::GenericAttribute::toFloat);
	testGenericAttributeSerialization<Util::_NumberAttribute<long>>(-234978l, "-234978", "numberLong", &Util::GenericAttribute::toInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned long>>(413214ul, "413214", "numberULong", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<int>>(-234978, "-234978", "numberInt", &Util::GenericAttribute::toInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned int>>(413214u, "413214", "numberUInt", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<short>>(-1200, "-1200", "numberShort", &Util::GenericAttribute::toInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned short>>(837u, "837", "numberUShort", &Util::GenericAttribute::toUnsignedInt);

	testGenericAttributeSerialization<Util::_NumberAttribute<char>>(-128, "-128", "numberByte", &Util::GenericAttribute::toInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<char>>(0, "0", "numberByte", &Util::GenericAttribute::toInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<char>>(127, "127", "numberByte", &Util::GenericAttribute::toInt);

	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned char>>(0u, "0", "numberUByte", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned char>>(127u, "127", "numberUByte", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<Util::_NumberAttribute<unsigned char>>(255u, "255", "numberUByte", &Util::GenericAttribute::toUnsignedInt);

	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("Hello, world!", "Hello, world!", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("abc", "abc", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("ABC", "ABC", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("123", "123", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("x", "x", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("", "", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("\"\"", "\\\"\\\"", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("''", "''", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("'\"", "'\\\"", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("[1, 2, 3]", "[1, 2, 3]", "string", &Util::GenericAttribute::toString);
	testGenericAttributeSerialization<Util::_StringAttribute<std::string>>("a\nb\nc", "a\\nb\\nc", "string", &Util::GenericAttribute::toString);
}

static void checkGenericAttributeListsEqual(const Util::GenericAttributeList * expected, const Util::GenericAttributeList * actual) {
	CPPUNIT_ASSERT(expected != nullptr);
	CPPUNIT_ASSERT(actual != nullptr);
	CPPUNIT_ASSERT_EQUAL(expected->size(), actual->size());
	auto actualIt = actual->begin();
	for(auto expectedIt = expected->begin(); expectedIt != expected->end(); ++expectedIt) {
		CPPUNIT_ASSERT(typeid(**expectedIt) == typeid(**actualIt));
		CPPUNIT_ASSERT_EQUAL((*expectedIt)->toJSON(), (*actualIt)->toJSON());
		++actualIt;
	}
}

void GenericAttributeSerializationTest::testListSerialization() {
	std::unique_ptr<Util::GenericAttributeList> container(new Util::GenericAttributeList);
	container->push_back(new Util::BoolAttribute(true));
	container->push_back(new Util::_StringAttribute<std::string>("[1, 2, \"xyz\"]"));
	container->push_back(new Util::_NumberAttribute<short>(12345));
	container->push_back(new Util::BoolAttribute(false));
	
	Util::GenericAttributeList * innerList = new Util::GenericAttributeList;
	innerList->push_back(new Util::_StringAttribute<std::string>("one"));
	innerList->push_back(new Util::_StringAttribute<std::string>("two"));
	innerList->push_back(new Util::_StringAttribute<std::string>("three"));
	container->push_back(innerList);
	
	const auto content = Util::GenericAttributeSerialization::serialize(container.get());
	
	std::unique_ptr<Util::GenericAttribute> attribute(Util::GenericAttributeSerialization::unserialize(content));
	CPPUNIT_ASSERT(typeid(*container) == typeid(*attribute));
	Util::GenericAttributeList * newContainer = dynamic_cast<Util::GenericAttributeList *>(attribute.get());
	checkGenericAttributeListsEqual(container.get(), newContainer);
}

static void checkGenericAttributeMapsEqual(const Util::GenericAttributeMap * expected, const Util::GenericAttributeMap * actual) {
	CPPUNIT_ASSERT(expected != nullptr);
	CPPUNIT_ASSERT(actual != nullptr);
	CPPUNIT_ASSERT_EQUAL(expected->size(), actual->size());
	auto actualIt = actual->begin();
	for(auto expectedIt = expected->begin(); expectedIt != expected->end(); ++expectedIt) {
		CPPUNIT_ASSERT(typeid(*(expectedIt->second)) == typeid(*(actualIt->second)));
		CPPUNIT_ASSERT(expectedIt->first == actualIt->first);
		CPPUNIT_ASSERT_EQUAL(expectedIt->second->toJSON(), actualIt->second->toJSON());
		++actualIt;
	}
}

void GenericAttributeSerializationTest::testMapSerialization() {
	std::unique_ptr<Util::GenericAttributeMap> container(new Util::GenericAttributeMap);
	container->setValue(Util::StringIdentifier("firstBool"), new Util::BoolAttribute(true));
	container->setValue(Util::StringIdentifier("secondBool"), new Util::BoolAttribute(false));
	container->setValue(Util::StringIdentifier("first string"), new Util::_StringAttribute<std::string>("[1, 2, \"xyz\"]"));
	container->setValue(Util::StringIdentifier("second string"), new Util::_StringAttribute<std::string>("hello"));
	container->setValue(Util::StringIdentifier("first number"), new Util::_NumberAttribute<short>(12345));
	container->setValue(Util::StringIdentifier("second number"), new Util::_NumberAttribute<long>(-987654321));
	
	Util::GenericAttributeMap * innerMap = new Util::GenericAttributeMap;
	innerMap->setValue(Util::StringIdentifier("a"), new Util::_StringAttribute<std::string>("one"));
	innerMap->setValue(Util::StringIdentifier("b"), new Util::_StringAttribute<std::string>("two"));
	innerMap->setValue(Util::StringIdentifier("c"), new Util::_StringAttribute<std::string>("three"));
	container->setValue(Util::StringIdentifier("innerMap"), innerMap);
	
	const auto content = Util::GenericAttributeSerialization::serialize(container.get());
	
	std::unique_ptr<Util::GenericAttribute> attribute(Util::GenericAttributeSerialization::unserialize(content));
	CPPUNIT_ASSERT(typeid(*container) == typeid(*attribute));
	Util::GenericAttributeMap * newContainer = dynamic_cast<Util::GenericAttributeMap *>(attribute.get());
	checkGenericAttributeMapsEqual(container.get(), newContainer);
}

void GenericAttributeSerializationTest::testNestedSerialization() {
	std::unique_ptr<Util::GenericAttributeMap> container(new Util::GenericAttributeMap);
	{
		Util::GenericAttributeList * innerMapInnerList = new Util::GenericAttributeList;
		innerMapInnerList->push_back(new Util::_StringAttribute<std::string>("alpha"));
		innerMapInnerList->push_back(new Util::_StringAttribute<std::string>("beta"));
		innerMapInnerList->push_back(new Util::_StringAttribute<std::string>("gamma"));
		innerMapInnerList->push_back(new Util::_StringAttribute<std::string>("delta"));
		
		Util::GenericAttributeMap * innerMap = new Util::GenericAttributeMap;
		innerMap->setValue(Util::StringIdentifier("a"), new Util::_StringAttribute<std::string>("one"));
		innerMap->setValue(Util::StringIdentifier("b"), new Util::_StringAttribute<std::string>("two"));
		innerMap->setValue(Util::StringIdentifier("c"), new Util::_StringAttribute<std::string>("three"));
		innerMap->setValue(Util::StringIdentifier("greek"), innerMapInnerList);
		
		Util::GenericAttributeMap * innerListInnerMap = new Util::GenericAttributeMap;
		innerListInnerMap->setValue(Util::StringIdentifier("1"), new Util::_StringAttribute<std::string>("one"));
		innerListInnerMap->setValue(Util::StringIdentifier("2"), new Util::_StringAttribute<std::string>("two"));
		innerListInnerMap->setValue(Util::StringIdentifier("3"), new Util::_StringAttribute<std::string>("three"));
		innerListInnerMap->setValue(Util::StringIdentifier("4"), new Util::_StringAttribute<std::string>("four"));
		innerListInnerMap->setValue(Util::StringIdentifier("5"), new Util::_StringAttribute<std::string>("five"));
		
		Util::GenericAttributeList * innerList = new Util::GenericAttributeList;
		innerList->push_back(new Util::_StringAttribute<std::string>("one"));
		innerList->push_back(new Util::_StringAttribute<std::string>("two"));
		innerList->push_back(innerListInnerMap);
		innerList->push_back(new Util::_StringAttribute<std::string>("four"));
		
		container->setValue(Util::StringIdentifier("a"), new Util::_StringAttribute<std::string>("string with key a"));
		container->setValue(Util::StringIdentifier("b"), innerMap);
		container->setValue(Util::StringIdentifier("c"), new Util::_StringAttribute<std::string>("string with key c"));
		container->setValue(Util::StringIdentifier("d"), innerList);
		container->setValue(Util::StringIdentifier("e"), new Util::_StringAttribute<std::string>("string with key e"));
	}
	
	const auto content = Util::GenericAttributeSerialization::serialize(container.get());
	
	std::unique_ptr<Util::GenericAttribute> attribute(Util::GenericAttributeSerialization::unserialize(content));
	CPPUNIT_ASSERT(typeid(*container) == typeid(*attribute));
	Util::GenericAttributeMap * newContainer = dynamic_cast<Util::GenericAttributeMap *>(attribute.get());
	checkGenericAttributeMapsEqual(container.get(), newContainer);

	Util::GenericAttributeMap * newInnerMap = dynamic_cast<Util::GenericAttributeMap *>(newContainer->getValue(Util::StringIdentifier("b")));
	CPPUNIT_ASSERT(newInnerMap != nullptr);
	Util::GenericAttributeList * newInnerMapInnerList = dynamic_cast<Util::GenericAttributeList *>(newInnerMap->getValue(Util::StringIdentifier("greek")));
	CPPUNIT_ASSERT(newInnerMapInnerList != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("alpha"), newInnerMapInnerList->front()->toString());
	CPPUNIT_ASSERT_EQUAL(std::string("delta"), newInnerMapInnerList->back()->toString());
	
	Util::GenericAttributeList * newInnerList = dynamic_cast<Util::GenericAttributeList *>(newContainer->getValue(Util::StringIdentifier("d")));
	CPPUNIT_ASSERT(newInnerList != nullptr);
	Util::GenericAttributeMap * newInnerListInnerMap = dynamic_cast<Util::GenericAttributeMap *>(newInnerList->at(2));
	CPPUNIT_ASSERT(newInnerListInnerMap != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("four"), newInnerListInnerMap->getString(Util::StringIdentifier("4"))); 
}

class CustomGenericAttribute : public Util::GenericAttribute {
	public:
		typedef std::pair<unsigned long, unsigned long> content_t;
	private:
		content_t _content;
	public:
		CustomGenericAttribute(const content_t & content) : _content(content) {
		}

		const content_t & getContent() const {
			return _content;
		}

		unsigned int toUnsignedInt() const override {
			return _content.first + _content.second;
		}

		CustomGenericAttribute * clone() const override {
			return new CustomGenericAttribute(*this);
		}
};


const static std::string GATypeNameCGA = "customGenericAttribute";
static std::pair<std::string, std::string> serializeCGA(const std::pair<const Util::GenericAttribute *, const Util::GenericAttributeMap *> & attributeAndContext) {
	auto customAttribute = dynamic_cast<const CustomGenericAttribute *>(attributeAndContext.first);
	std::ostringstream stream;
	auto content = customAttribute->getContent();
	stream << content.first << ' ' << content.second;
	return std::make_pair(GATypeNameCGA, stream.str());
}
static CustomGenericAttribute * unserializeCGA(const std::pair<std::string, const Util::GenericAttributeMap *> & contentAndContext) {
	unsigned long a;
	unsigned long b;
	std::istringstream stream(contentAndContext.first);
	stream >> a >> b;
	return new CustomGenericAttribute(std::make_pair(a, b));
}

class CustomWithOperatorsGenericAttribute : public CustomGenericAttribute {
	public:
		CustomWithOperatorsGenericAttribute(const content_t & content) : CustomGenericAttribute(content) {
		}

		friend std::ostream & operator<<(std::ostream & out, const CustomWithOperatorsGenericAttribute & attrib) {
			auto content = attrib.getContent();
			return out << content.first << ' ' << content.second;
		}
		friend std::istream & operator>>(std::istream & in, CustomWithOperatorsGenericAttribute & attrib) {
			unsigned long a;
			unsigned long b;
			in >> a >> b;
			attrib = CustomWithOperatorsGenericAttribute(std::make_pair(a, b));
			return in;
		}

		CustomWithOperatorsGenericAttribute * clone() const override {
			return new CustomWithOperatorsGenericAttribute(*this);
		}
};

const static std::string GATypeNameCWOGA = "customWithOperatorsGenericAttribute";
static std::pair<std::string, std::string> serializeCWOGA(const std::pair<const Util::GenericAttribute *, const Util::GenericAttributeMap *> & attributeAndContext) {
	auto customAttribute = dynamic_cast<const CustomWithOperatorsGenericAttribute *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << *customAttribute;
	return std::make_pair(GATypeNameCWOGA, stream.str());
}
static CustomWithOperatorsGenericAttribute * unserializeCWOGA(const std::pair<std::string, const Util::GenericAttributeMap *> & contentAndContext) {
	auto customAttribute = new CustomWithOperatorsGenericAttribute(std::make_pair(0, 0));
	std::istringstream stream(contentAndContext.first);
	stream >> *customAttribute;
	return customAttribute;
}

void GenericAttributeSerializationTest::testCustomSerialization() {
	Util::GenericAttributeSerialization::registerSerializer<CustomGenericAttribute>(GATypeNameCGA, serializeCGA, unserializeCGA);
	Util::GenericAttributeSerialization::registerSerializer<CustomWithOperatorsGenericAttribute>(GATypeNameCWOGA, serializeCWOGA, unserializeCWOGA);

	testGenericAttributeSerialization<CustomGenericAttribute>(std::make_pair(0, 0), "0 0", "customGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<CustomGenericAttribute>(std::make_pair(17, 42), "17 42", "customGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<CustomGenericAttribute>(std::make_pair(123456789, 987654321), "123456789 987654321", "customGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
	
	testGenericAttributeSerialization<CustomWithOperatorsGenericAttribute>(std::make_pair(0, 0), "0 0", "customWithOperatorsGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<CustomWithOperatorsGenericAttribute>(std::make_pair(17, 42), "17 42", "customWithOperatorsGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
	testGenericAttributeSerialization<CustomWithOperatorsGenericAttribute>(std::make_pair(123456789, 987654321), "123456789 987654321", "customWithOperatorsGenericAttribute", &Util::GenericAttribute::toUnsignedInt);
}

class NoDefaultConstructorGenericAttribute : public Util::GenericAttribute {
	private:
		const Util::GenericAttributeMap * exporter;
		
		const Util::GenericAttributeMap * importer;

		std::vector<std::string> list;

	public:
		NoDefaultConstructorGenericAttribute(const std::tuple<const Util::GenericAttributeMap *, const Util::GenericAttributeMap *, const std::vector<std::string>> & convertersAndValues) : 
			exporter(std::get<0>(convertersAndValues)), importer(std::get<1>(convertersAndValues)), list(std::get<2>(convertersAndValues)) {
		}

		friend std::ostream & operator<<(std::ostream & out, const NoDefaultConstructorGenericAttribute & attrib) {
			out << attrib.list.size();
			for(const auto & element : attrib.list) {
				out << ' ' << attrib.exporter->getString(element);
			}
			return out;
		}
		friend std::istream & operator>>(std::istream & in, NoDefaultConstructorGenericAttribute & attrib) {
			std::size_t numElements;
			in >> numElements;
			attrib.list.clear();
			attrib.list.reserve(numElements);
			for(std::size_t i = 0; i < numElements; ++i) {
				std::string element;
				in >> element;
				attrib.list.push_back(attrib.importer->getString(element));
			}
			return in;
		}

		std::string toString() const override {
			std::ostringstream stream;
			stream << list.size();
			for(const auto & element : list) {
				stream << ' ' << element;
			}
			return stream.str();
		}

		NoDefaultConstructorGenericAttribute * clone() const override {
			return new NoDefaultConstructorGenericAttribute(*this);
		}
};

const static std::string GATypeNameNDCGA = "noDefaultConstructorGenericAttribute";
static std::pair<std::string, std::string> serializeNDCGA(const std::pair<const Util::GenericAttribute *, const Util::GenericAttributeMap *> & attributeAndContext) {
	auto customAttribute = dynamic_cast<const NoDefaultConstructorGenericAttribute *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << *customAttribute;
	return std::make_pair(GATypeNameNDCGA, stream.str());
}
static NoDefaultConstructorGenericAttribute * unserializeNDCGA(const std::pair<std::string, const Util::GenericAttributeMap *> & contentAndContext) {
	CPPUNIT_ASSERT(contentAndContext.second != nullptr);
	auto exporter = dynamic_cast<Util::GenericAttributeMap *>(contentAndContext.second->getValue(Util::StringIdentifier("exporter")));
	CPPUNIT_ASSERT(exporter != nullptr);
	auto importer = dynamic_cast<Util::GenericAttributeMap *>(contentAndContext.second->getValue(Util::StringIdentifier("importer")));
	CPPUNIT_ASSERT(importer != nullptr);
	auto customAttribute = new NoDefaultConstructorGenericAttribute(std::make_tuple(exporter, importer, std::vector<std::string>()));
	std::istringstream stream(contentAndContext.first);
	stream >> *customAttribute;
	return customAttribute;
}

void GenericAttributeSerializationTest::testCustomWithContextSerialization() {
	Util::GenericAttributeMap * exporterGerman = new Util::GenericAttributeMap;
	exporterGerman->setString(Util::StringIdentifier("one"), "eins");
	exporterGerman->setString(Util::StringIdentifier("two"), "zwei");
	exporterGerman->setString(Util::StringIdentifier("three"), "drei");
	exporterGerman->setString(Util::StringIdentifier("four"), "vier");
	std::unique_ptr<Util::GenericAttributeMap> exporterFrench(new Util::GenericAttributeMap);
	exporterFrench->setString(Util::StringIdentifier("one"), "un");
	exporterFrench->setString(Util::StringIdentifier("two"), "deux");
	exporterFrench->setString(Util::StringIdentifier("three"), "trois");
	exporterFrench->setString(Util::StringIdentifier("four"), "quatre");
	Util::GenericAttributeMap * importer = new Util::GenericAttributeMap;
	importer->setString(Util::StringIdentifier("eins"), "one");
	importer->setString(Util::StringIdentifier("zwei"), "two");
	importer->setString(Util::StringIdentifier("drei"), "three");
	importer->setString(Util::StringIdentifier("vier"), "four");
	importer->setString(Util::StringIdentifier("un"), "one");
	importer->setString(Util::StringIdentifier("deux"), "two");
	importer->setString(Util::StringIdentifier("trois"), "three");
	importer->setString(Util::StringIdentifier("quatre"), "four");

	std::unique_ptr<Util::GenericAttributeMap> context(new Util::GenericAttributeMap);
	context->setValue(Util::StringIdentifier("exporter"), exporterGerman);
	context->setValue(Util::StringIdentifier("importer"), importer);

	Util::GenericAttributeSerialization::registerSerializer<NoDefaultConstructorGenericAttribute>(GATypeNameNDCGA, serializeNDCGA, unserializeNDCGA);

	std::vector<std::string> testOne;
	testOne.emplace_back("two");
	testOne.emplace_back("four");
	testOne.emplace_back("three");
	testOne.emplace_back("one");
	testGenericAttributeSerialization<NoDefaultConstructorGenericAttribute>(std::make_tuple(exporterGerman, importer, testOne), 
																			"4 zwei vier drei eins", 
																			"noDefaultConstructorGenericAttribute", &Util::GenericAttribute::toString, context.get());
	testGenericAttributeSerialization<NoDefaultConstructorGenericAttribute>(std::make_tuple(exporterFrench.get(), importer, testOne), 
																			"4 deux quatre trois un", 
																			"noDefaultConstructorGenericAttribute", &Util::GenericAttribute::toString, context.get());

	std::vector<std::string> testTwo;
	testTwo.emplace_back("two");
	testTwo.emplace_back("two");
	testTwo.emplace_back("one");
	testTwo.emplace_back("three");
	testTwo.emplace_back("four");
	testGenericAttributeSerialization<NoDefaultConstructorGenericAttribute>(std::make_tuple(exporterGerman, importer, testTwo), 
																			"5 zwei zwei eins drei vier", 
																			"noDefaultConstructorGenericAttribute", &Util::GenericAttribute::toString, context.get());
	testGenericAttributeSerialization<NoDefaultConstructorGenericAttribute>(std::make_tuple(exporterFrench.get(), importer, testTwo), 
																			"5 deux deux un trois quatre", 
																			"noDefaultConstructorGenericAttribute", &Util::GenericAttribute::toString, context.get());
}
