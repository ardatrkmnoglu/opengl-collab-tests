static const char* test_procedure = "BufferObjects_BindBuffer_TP_001";
static const char* test_case_1 = "BufferObjects_BindBuffer_TC_001";
static const char* test_case_2 = "BufferObjects_BindBuffer_TC_002";


TEST_LOG_FAIL(test_case_, test_procedure, "error = 0x%x.", err);

TEST_LOG_SUCCESS(test_case_, test_procedure);

TEST_LOG_INFO(fmt, ...)


    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_, test_procedure);
    }
    else {
         TEST_LOG_FAIL(test_case_, test_procedure, "error = 0x%x.", err);
    }


döngü varsa:

        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_, test_procedure, "error = 0x%x.", err);
            return;
        }


GL_INVALID_VALUE

GL_INVALID_OPERATION

GL_NO_ERROR

