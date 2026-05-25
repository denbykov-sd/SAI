#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sai.h"

static sai_buffer_api_t* get_new_buffer_api();

// intentionally unscoped macro
#define PREPARE_FOR_TESTING()    \
    printf("\n");          \
    printf("%s\n", __func__); \
    printf("\n");          \
    fflush(stdout);        \
    sai_buffer_api_t* api = get_new_buffer_api(); \
    sai_status_t status = SAI_STATUS_SUCCESS;

// intentionally unscoped macro
#define PASS_ATTRIBUTES(attr_list) \
    sizeof((attr_list)) / sizeof(sai_attribute_t), (attr_list)

#define TEST_CASE(desc) \
    for (int _i = (printf("- %s:\n", #desc), fflush(stdout), 0); _i < 1; _i++, printf("  --> [PASS]\n"))

#define UNUSED(x) (void)(x)

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    UNUSED(profile_id);
    UNUSED(variable);

    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    UNUSED(profile_id);
    UNUSED(variable);
    UNUSED(value);

    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

typedef struct test_object_id_s {
    sai_uint8_t  object_type;
    sai_uint8_t  reserved[3];
    sai_uint32_t data;
} test_object_id_t;

sai_buffer_api_t* get_new_buffer_api()
{
    sai_api_uninitialize();
    sai_buffer_api_t *api = NULL;
    sai_status_t status = 0;

    status = sai_api_initialize(0, &test_services);
    assert(status == SAI_STATUS_SUCCESS);

    status = sai_api_query(SAI_API_BUFFERS, (void**)&api);
    assert(status == SAI_STATUS_SUCCESS);

    return api;
}

void test_create_buffer_pool() {
    PREPARE_FOR_TESTING();

    sai_object_id_t pool_id = 0;

    TEST_CASE(missing mandatory attributes) {
        status = api->create_buffer_pool(
        &pool_id, 0, NULL);
        assert(status == SAI_STATUS_MANDATORY_ATTRIBUTE_MISSING);
    }

    TEST_CASE(passing an invalid attribute) {
        sai_attribute_t attr_list[1] = {0};  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = 123;
        
        status = api->create_buffer_pool(
            &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
    }

    TEST_CASE(missing second mandatary param) {
        sai_attribute_t attr_list[1] = {0};  
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_MANDATORY_ATTRIBUTE_MISSING);
    }

    TEST_CASE(happy pass) {
        sai_attribute_t attr_list[2] = {0};  
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_SUCCESS);

        assert(sai_object_type_query(pool_id) == SAI_OBJECT_TYPE_BUFFER_POOL);

        sai_attribute_t get_attr_list[3] = {0};
        get_attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        get_attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        get_attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;

        api->get_buffer_pool_attr(pool_id, PASS_ATTRIBUTES(get_attr_list));
        assert(status == SAI_STATUS_SUCCESS);

        assert(get_attr_list[0].value.s32 == attr_list[0].value.s32);
        assert(get_attr_list[1].value.u32 == attr_list[1].value.u32);
        assert(get_attr_list[2].value.s32 == SAI_BUFFER_THRESHOLD_MODE_DYNAMIC);
    }

    TEST_CASE(invalide value for SAI_BUFFER_POOL_ATTR_TYPE) {
        sai_attribute_t attr_list[2] = {0};  
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS + 1000;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0 + 0);
    }

    TEST_CASE(invalide value for SAI_BUFFER_POOL_ATTR_TYPE) {
        sai_attribute_t attr_list[3] = {0};  
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_EGRESS;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;

        attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;
        attr_list[2].value.s32 = SAI_BUFFER_THRESHOLD_MODE_DYNAMIC + 1000;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0 + 2);
    }

    TEST_CASE(happy pass 2) {
        sai_attribute_t attr_list[3] = {0};  
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_EGRESS;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;

        attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;
        attr_list[2].value.s32 = SAI_BUFFER_THRESHOLD_MODE_STATIC;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_SUCCESS);

        sai_attribute_t get_attr_list[3] = {0};
        get_attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        get_attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        get_attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;

        api->get_buffer_pool_attr(pool_id, PASS_ATTRIBUTES(get_attr_list));
        assert(status == SAI_STATUS_SUCCESS);

        assert(get_attr_list[0].value.s32 == attr_list[0].value.s32);
        assert(get_attr_list[1].value.u32 == attr_list[1].value.u32);
        assert(get_attr_list[2].value.s32 == SAI_BUFFER_THRESHOLD_MODE_STATIC);
    }
}

void test_create_buffer_pool_capacity() {
    PREPARE_FOR_TESTING();

    sai_object_id_t pool_id = 0;
    int capacity = 4;

    TEST_CASE(fill up to the capacity) {
        for (int i = 0; i < capacity; i++) {
            sai_attribute_t attr_list[2] = {0};
            attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
            attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

            attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
            attr_list[1].value.u32 = 123;
            
            status = api->create_buffer_pool(
            &pool_id, PASS_ATTRIBUTES(attr_list));
            assert(status == SAI_STATUS_SUCCESS);
        }
    }

    TEST_CASE(go over the capacity) {
        sai_attribute_t attr_list[2] = {0};
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_TABLE_FULL);
    }

    TEST_CASE(remove an item and create it again) {
        status = api->remove_buffer_pool(pool_id);
        assert(status == SAI_STATUS_SUCCESS);

        sai_attribute_t attr_list[2] = {0};
        attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

        attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr_list[1].value.u32 = 123;
        
        status = api->create_buffer_pool(
        &pool_id, PASS_ATTRIBUTES(attr_list));
        assert(status == SAI_STATUS_SUCCESS);
    }
}

void test_set_buffer_attribute() {
    PREPARE_FOR_TESTING();

    sai_object_id_t pool_id = 0;
    sai_attribute_t attr_list[2] = {0};
    attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
    attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

    attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
    attr_list[1].value.u32 = 123;
    
    status = api->create_buffer_pool(
    &pool_id, PASS_ATTRIBUTES(attr_list));
    assert(status == SAI_STATUS_SUCCESS);

    TEST_CASE(set type) {
        sai_attribute_t attr = {0};
        attr.id = SAI_BUFFER_POOL_ATTR_TYPE;
        attr.value.s32 = SAI_BUFFER_POOL_INGRESS;

        status = api->set_buffer_pool_attr(pool_id, &attr);
        assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
    }

    TEST_CASE(set size) {
        sai_attribute_t attr = {0};
        attr.id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr.value.u32 = 12821;

        status = api->set_buffer_pool_attr(pool_id, &attr);
        assert(status == SAI_STATUS_SUCCESS);

        sai_attribute_t get_attr_list[3] = {0};
        get_attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        get_attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        get_attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;

        api->get_buffer_pool_attr(pool_id, PASS_ATTRIBUTES(get_attr_list));
        assert(status == SAI_STATUS_SUCCESS);

        assert(get_attr_list[0].value.s32 == attr_list[0].value.s32);
        assert(get_attr_list[1].value.u32 == attr.value.u32);
        assert(get_attr_list[2].value.s32 == SAI_BUFFER_THRESHOLD_MODE_DYNAMIC);
    }

    TEST_CASE(bad buffer pool) {
        sai_attribute_t attr = {0};
        attr.id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr.value.u32 = 12821;

        test_object_id_t bad_pool_id_t = (test_object_id_t){
            .object_type = SAI_OBJECT_TYPE_BUFFER_POOL,
            .data = 1
        };

        sai_object_id_t bad_pool_id = *(sai_object_id_t*)&bad_pool_id_t;

        status = api->set_buffer_pool_attr(
            bad_pool_id,
            &attr);
        assert(status == SAI_STATUS_INVALID_OBJECT_ID);
    }

    TEST_CASE(set th mode) {
        sai_attribute_t attr = {0};
        attr.id = SAI_BUFFER_POOL_ATTR_TH_MODE;
        attr.value.s32 = SAI_BUFFER_THRESHOLD_MODE_DYNAMIC;

        status = api->set_buffer_pool_attr(pool_id, &attr);
        assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
    }

    // invalidate pool id
    status = api->remove_buffer_pool(pool_id);
    assert(status == SAI_STATUS_SUCCESS);

    TEST_CASE(set valid attribute of the deleted object) {
        sai_attribute_t attr = {0};
        attr.id = SAI_BUFFER_POOL_ATTR_SIZE;
        attr.value.u32 = 12821;

        status = api->set_buffer_pool_attr(pool_id, &attr);
        assert(status == SAI_STATUS_INVALID_OBJECT_ID);
    }
}

void test_get_buffer_attribute() {
    PREPARE_FOR_TESTING();

    sai_object_id_t pool_id = 0;
    sai_attribute_t attr_list[2] = {0};
    attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
    attr_list[0].value.s32 = SAI_BUFFER_POOL_INGRESS;

    attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
    attr_list[1].value.u32 = 123;
    
    status = api->create_buffer_pool(
    &pool_id, PASS_ATTRIBUTES(attr_list));
    assert(status == SAI_STATUS_SUCCESS);

    // invalidate pool id
    status = api->remove_buffer_pool(pool_id);
    assert(status == SAI_STATUS_SUCCESS);

    TEST_CASE(get attributs of the deleted object) {
        sai_attribute_t get_attr_list[3] = {0};
        get_attr_list[0].id = SAI_BUFFER_POOL_ATTR_TYPE;
        get_attr_list[1].id = SAI_BUFFER_POOL_ATTR_SIZE;
        get_attr_list[2].id = SAI_BUFFER_POOL_ATTR_TH_MODE;

        status = api->get_buffer_pool_attr(pool_id, PASS_ATTRIBUTES(get_attr_list));
        assert(status == SAI_STATUS_INVALID_OBJECT_ID);
    }
}

int main()
{
    test_create_buffer_pool();
    test_create_buffer_pool_capacity();
    test_set_buffer_attribute();
    test_get_buffer_attribute();

    return 0;
}