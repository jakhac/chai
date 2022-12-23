#include "testFixtures.h"

using namespace TTTest;
using namespace TT;


TEST_F(TTTest, TestMasking) {

    int msb;
    uint32_t ones = ~(0ULL);
    uint64_t totalBytes, numBucketsPossible;

    // getTTIndex should never return an index greater than the msb
    int mbSize = DEFAULT_TT_SIZE;
    getIndexMSB(mbSize, &msb, &totalBytes, &numBucketsPossible);
    ASSERT_GE(getTTIndex(ones), msb << 1);

    mbSize = 128;
    resizeHashTables(mbSize);
    getIndexMSB(mbSize, &msb, &totalBytes, &numBucketsPossible);
    ASSERT_GE(getTTIndex(ones), msb << 1);

    mbSize = 32;
    resizeHashTables(mbSize);
    getIndexMSB(mbSize, &msb, &totalBytes, &numBucketsPossible);
    ASSERT_GE(getTTIndex(ones), msb << 1);

}
