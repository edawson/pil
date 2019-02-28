#ifndef DICTIONARY_BUILDER_TEST_H_
#define DICTIONARY_BUILDER_TEST_H_

#include "dictionary_builder.h"
#include <gtest/gtest.h>

namespace pil {

// uint32t
TEST(DictionaryBuilderTests, FindSingleUInt) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    cbuild->Append(10);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<uint32_t>(10));
}

TEST(DictionaryBuilderTests, FindSingleUIntFail) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    cbuild->Append(10);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(0, dict.Contains<uint32_t>(5));
}

TEST(DictionaryBuilderTests, GetSingleUInt) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    cbuild->Append(10);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<uint32_t>(10));
    ASSERT_EQ(10, dict.Get<uint32_t>(0));
}

TEST(DictionaryBuilderTests, GetSingleUIntFail) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    cbuild->Append(10);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<uint32_t>(10));
    ASSERT_NE(25, dict.Get<uint32_t>(0));
}

// int32t
TEST(DictionaryBuilderTests, FindSingleInt) {
    std::shared_ptr< ColumnSetBuilder<int32_t> > cbuild = std::make_shared< ColumnSetBuilder<int32_t> >();

    cbuild->Append(-10);

    NumericDictionaryBuilder<int32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<int32_t>(-10));
}

TEST(DictionaryBuilderTests, FindSingleIntFail) {
    std::shared_ptr< ColumnSetBuilder<int32_t> > cbuild = std::make_shared< ColumnSetBuilder<int32_t> >();

    cbuild->Append(-10);

    NumericDictionaryBuilder<int32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(0, dict.Contains<int32_t>(-5));
}

TEST(DictionaryBuilderTests, GetSingleInt) {
    std::shared_ptr< ColumnSetBuilder<int32_t> > cbuild = std::make_shared< ColumnSetBuilder<int32_t> >();

    cbuild->Append(-10);

    NumericDictionaryBuilder<int32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<int32_t>(-10));
    ASSERT_EQ(-10, dict.Get<int32_t>(0));
}

TEST(DictionaryBuilderTests, GetSingleIntFail) {
    std::shared_ptr< ColumnSetBuilder<int32_t> > cbuild = std::make_shared< ColumnSetBuilder<int32_t> >();

    cbuild->Append(-10);

    NumericDictionaryBuilder<int32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<int32_t>(-10));
    ASSERT_NE(-25, dict.Get<int32_t>(0));
}

// float
TEST(DictionaryBuilderTests, FindSingleFloat) {
    std::shared_ptr< ColumnSetBuilder<float> > cbuild = std::make_shared< ColumnSetBuilder<float> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<float> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_DOUBLE_EQ(1, dict.Contains<float>(10.5));
}

TEST(DictionaryBuilderTests, FindSingleFloatFail) {
    std::shared_ptr< ColumnSetBuilder<float> > cbuild = std::make_shared< ColumnSetBuilder<float> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<float> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_DOUBLE_EQ(0, dict.Contains<float>(44.3));
}

TEST(DictionaryBuilderTests, GetSingleFloat) {
    std::shared_ptr< ColumnSetBuilder<float> > cbuild = std::make_shared< ColumnSetBuilder<float> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<float>(10.5));
    ASSERT_DOUBLE_EQ(10.5, dict.Get<float>(0));
}

// double
TEST(DictionaryBuilderTests, FindSingleDouble) {
    std::shared_ptr< ColumnSetBuilder<double> > cbuild = std::make_shared< ColumnSetBuilder<double> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_DOUBLE_EQ(1, dict.Contains<double>(10.5));
}

TEST(DictionaryBuilderTests, FindSingleDoubleFail) {
    std::shared_ptr< ColumnSetBuilder<double> > cbuild = std::make_shared< ColumnSetBuilder<double> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_DOUBLE_EQ(0, dict.Contains<double>(44.3));
}

TEST(DictionaryBuilderTests, GetSingleDouble) {
    std::shared_ptr< ColumnSetBuilder<double> > cbuild = std::make_shared< ColumnSetBuilder<double> >();

    cbuild->Append(10.5);

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<double>(10.5));
    ASSERT_DOUBLE_EQ(10.5, dict.Get<double>(0));
}


//
TEST(DictionaryBuilderTests, FindSingleLargeSet) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    for(int i = 0; i < 1000; ++i)
        cbuild->Append(i);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1000, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<uint32_t>(541));
}

TEST(DictionaryBuilderTests, FindSingleLargeSetFail) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    for(int i = 0; i < 1000; ++i)
        cbuild->Append(i);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1000, dict.NumberRecords());
    ASSERT_EQ(0, dict.Contains<uint32_t>(2999));
}

TEST(DictionaryBuilderTests, GetSingleLargeSet) {
    std::shared_ptr< ColumnSetBuilder<uint32_t> > cbuild = std::make_shared< ColumnSetBuilder<uint32_t> >();

    for(int i = 0; i < 1000; ++i)
        cbuild->Append(i);

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[0], true));
    ASSERT_EQ(1000, dict.NumberRecords());
    ASSERT_EQ(1, dict.Contains<uint32_t>(541));

    for(int i = 0; i < 1000; ++i)
        ASSERT_EQ(i, dict.Get<uint32_t>(i));
}

// tensors
// uint32t
TEST(DictionaryBuilderTests, FindSingleIntTensorAny) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_EQ(1, dict.Contains<uint32_t>(10));
}

TEST(DictionaryBuilderTests, FindSingleIntTensorAnyNone) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_EQ(0, dict.Contains<uint32_t>(241));
}

TEST(DictionaryBuilderTests, FindSingleIntTensorExact) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_EQ(1, dict.Contains<uint32_t>(&vals[0], vals.size()));
}

TEST(DictionaryBuilderTests, FindSingleIntTensorExactNone) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    std::vector<uint32_t> similar = {10,12,14,12}; // 21 changed to 12
    ASSERT_EQ(0, dict.Contains<uint32_t>(&similar[0], similar.size()));
}


TEST(DictionaryBuilderTests, FindSingleIntTensorExactLarge) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());
    vals = {12,14,16};
    cbuild->Append(vals.data(), vals.size());
    vals = {21,49};
    cbuild->Append(vals.data(), vals.size());
    vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(3, dict.NumberRecords());
    ASSERT_EQ(9, dict.NumberElements());
    vals = {21,49};
    ASSERT_EQ(1, dict.Contains<uint32_t>(&vals[0], vals.size()));
}

TEST(DictionaryBuilderTests, FindSingleIntTensorExactLargeNone) {
    std::shared_ptr< ColumnSetBuilderTensor<uint32_t> > cbuild = std::make_shared< ColumnSetBuilderTensor<uint32_t> >();

    std::vector<uint32_t> vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());
    vals = {12,14,16};
    cbuild->Append(vals.data(), vals.size());
    vals = {21,49};
    cbuild->Append(vals.data(), vals.size());
    vals = {10,12,14,21};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<uint32_t> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(3, dict.NumberRecords());
    ASSERT_EQ(9, dict.NumberElements());
    vals = {1451,12,15,1,7,85,21,12};
    ASSERT_EQ(0, dict.Contains<uint32_t>(&vals[0], vals.size()));
}

// doubles
TEST(DictionaryBuilderTests, FindSingleDoubleTensorAny) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_DOUBLE_EQ(1, dict.Contains<double>(10.3));
}

TEST(DictionaryBuilderTests, FindSingleDoubleTensorAnyNone) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_DOUBLE_EQ(0, dict.Contains<double>(241));
}

TEST(DictionaryBuilderTests, FindSingleDoubleTensorExact) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    ASSERT_EQ(1, dict.Contains<double>(&vals[0], vals.size()));
}

TEST(DictionaryBuilderTests, FindSingleDoubleTensorExactNone) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(1, dict.NumberRecords());
    ASSERT_EQ(4, dict.NumberElements());
    std::vector<double> similar = {10.3,12.4,14.121,21.83};; // 21.81 changed to 21.83
    ASSERT_EQ(0, dict.Contains<double>(&similar[0], similar.size()));
}


TEST(DictionaryBuilderTests, FindSingleDoubleTensorExactLarge) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());
    vals = {12.41,14.123,16.191};
    cbuild->Append(vals.data(), vals.size());
    vals = {21,49.41};
    cbuild->Append(vals.data(), vals.size());
    vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(3, dict.NumberRecords());
    ASSERT_EQ(9, dict.NumberElements());
    vals = {21,49.41};
    ASSERT_EQ(1, dict.Contains<double>(&vals[0], vals.size()));
}

TEST(DictionaryBuilderTests, FindSingleDoubleTensorExactLargeNone) {
    std::shared_ptr< ColumnSetBuilderTensor<double> > cbuild = std::make_shared< ColumnSetBuilderTensor<double> >();

    std::vector<double> vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());
    vals = {12.41,14.123,16.191};
    cbuild->Append(vals.data(), vals.size());
    vals = {21,49.41};
    cbuild->Append(vals.data(), vals.size());
    vals = {10.3,12.4,14.121,21.81};
    cbuild->Append(vals.data(), vals.size());

    NumericDictionaryBuilder<double> dict;
    ASSERT_EQ(1, dict.Encode(cbuild->columns[1], cbuild->columns[0], true));
    ASSERT_EQ(3, dict.NumberRecords());
    ASSERT_EQ(9, dict.NumberElements());
    vals = {21,49.44}; // 49.41 changed to 49.44
    ASSERT_EQ(0, dict.Contains<double>(&vals[0], vals.size()));
}

}


#endif /* DICTIONARY_BUILDER_TEST_H_ */
