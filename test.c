#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
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

uint64_t create_port_object(uint32_t id) {
    test_object_id_t res = {
        .object_type = SAI_OBJECT_TYPE_PORT,
        .data = id
    };

    return *(uint64_t*)&res;
}

void test_create_lag(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test create lag\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    sai_attribute_t attr_list[1];  
    attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
    attr_list[0].value.oid = lag_id;
    
    status = lag_api->create_lag(
        &lag_id, 1, attr_list);
    assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
}

void test_remove_lag(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test remove lag\n");
    printf("\n");

    sai_status_t status = 0;

    {
        sai_object_id_t lag_id = 0;
        status = lag_api->create_lag(
            &lag_id, 0, NULL);
        assert(status == SAI_STATUS_SUCCESS); 
        
        status = lag_api->remove_lag(lag_id);
        assert(status == SAI_STATUS_SUCCESS);

        status = lag_api->remove_lag(lag_id);
        assert(status == SAI_STATUS_INVALID_OBJECT_ID);
    }

    {
        sai_object_id_t lag_id = 0;
        status = lag_api->create_lag(
            &lag_id, 0, NULL);
        assert(status == SAI_STATUS_SUCCESS);

        {
            sai_object_id_t lag_member_id = 0;
            sai_attribute_t attr_list[2];
            attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
            attr_list[0].value.oid = lag_id;
            attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
            attr_list[1].value.oid = create_port_object(0);
            
            status = lag_api->create_lag_member(
                &lag_member_id, 2, attr_list);
            assert(status == SAI_STATUS_SUCCESS);
        }
        
        status = lag_api->remove_lag(lag_id);
        assert(status == SAI_STATUS_OBJECT_IN_USE);
    }
}

void test_set_lag_attribute(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test set lag attribute\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    sai_attribute_t attr;  
    attr.id = SAI_LAG_ATTR_PORT_LIST;
    
    sai_object_id_t list[1];
    attr.value.objlist.count = 1;
    attr.value.objlist.list = list;

    status = lag_api->set_lag_attribute(lag_id, &attr);
    assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
}

int compare_lists(sai_object_list_t* l1, sai_object_list_t* l2, size_t count) {
    if (l1->count != l2->count) {
        return 1;
    }

    return memcmp(
        l1->list,
        l2->list,
        sizeof(sai_object_id_t) * count);
}

void test_get_lag_attribute(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test get lag attribute\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);


    sai_attribute_t attr;  
    attr.id = SAI_LAG_ATTR_PORT_LIST;
    
    sai_object_id_t list[16] = {0};
    attr.value.objlist.count = 16;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_object_id_t expected_memory[16] = {0};

        sai_object_list_t expected_list;
        expected_list.count = 0;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    sai_object_id_t lag_member_id1 = 0;
    {
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);
        
        status = lag_api->create_lag_member(
            &lag_member_id1, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    sai_object_id_t lag_member_id2 = 0;
    {
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(1);
        
        status = lag_api->create_lag_member(
            &lag_member_id2, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    memset(list, 0, sizeof(list));
    attr.value.objlist.count = 16;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(0);
        expected_memory[1] = create_port_object(1);

        sai_object_list_t expected_list;
        expected_list.count = 2;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    memset(list, 0, sizeof(list));
    attr.value.objlist.count = 1;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_object_id_t expected_memory[16] = {0};

        sai_object_list_t expected_list;
        expected_list.count = 2;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    status = lag_api->remove_lag_member(lag_member_id1);
    assert(status == SAI_STATUS_SUCCESS);

    memset(list, 0, sizeof(list));
    attr.value.objlist.count = 16;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(1);

        sai_object_list_t expected_list;
        expected_list.count = 1;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    status = lag_api->remove_lag_member(lag_member_id2);
    assert(status == SAI_STATUS_SUCCESS);

    memset(list, 0, sizeof(list));
    attr.value.objlist.count = 0;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_object_id_t expected_memory[16] = {0};

        sai_object_list_t expected_list;
        expected_list.count = 0;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    status = lag_api->remove_lag(lag_id);
    assert(status == SAI_STATUS_SUCCESS);

    memset(list, 0, sizeof(list));
    attr.value.objlist.count = 0;
    attr.value.objlist.list = list;

    status = lag_api->get_lag_attribute(
        lag_id, 1, &attr);
    assert(status == SAI_STATUS_INVALID_OBJECT_ID);

    status = lag_api->get_lag_attribute(
        0x5000000002, 1, &attr);
    assert(status == SAI_STATUS_INVALID_OBJECT_ID);
}

void test_lag_capacity(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test lag capacity\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id1 = 0;
    sai_object_id_t lag_id2 = 0;
    sai_object_id_t lag_id3 = 0;
    sai_object_id_t lag_id4 = 0;
    sai_object_id_t lag_id5 = 0;
    sai_object_id_t lag_id6 = 0;

    status = lag_api->create_lag(
        &lag_id1, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id2, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id3, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id4, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id5, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id6, 0, NULL);
    assert(status == SAI_STATUS_TABLE_FULL);

    status = lag_api->remove_lag(lag_id2);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id6, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id2, 0, NULL);
    assert(status == SAI_STATUS_TABLE_FULL);

    status = lag_api->remove_lag(lag_id6);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id2, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);
}

void test_create_lag_member(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test create lag member\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS); 

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[1];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        
        status = lag_api->create_lag_member(
            &lag_member_id, 1, attr_list);
        assert(status == SAI_STATUS_MANDATORY_ATTRIBUTE_MISSING);
    }

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[1];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[0].value.oid = create_port_object(0);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 1, attr_list);
        assert(status == SAI_STATUS_MANDATORY_ATTRIBUTE_MISSING);
    }

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);

        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_INVALID_PORT_MEMBER);
    }

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = 12345;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_INVALID_PARAMETER);
    }

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(32);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_INVALID_PORT_NUMBER);
    }

    {
        sai_object_id_t lag_id = 0;

        status = lag_api->create_lag(
            &lag_id, 0, NULL);
        assert(status == SAI_STATUS_SUCCESS);

        status = lag_api->remove_lag(lag_id);
        assert(status == SAI_STATUS_SUCCESS);

        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(1);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_INVALID_PARAMETER);
    }
}

void test_remove_lag_member(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test remove lag member\n");
    printf("\n");

    sai_status_t status = 0;
    sai_object_id_t lag_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS); 

    {
        sai_object_id_t lag_member_id = 0;
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[0].value.oid = lag_id;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);
        
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);

        status = lag_api->remove_lag_member(lag_member_id);
        assert(status == SAI_STATUS_SUCCESS);

        status = lag_api->remove_lag_member(lag_member_id);
        assert(status == SAI_STATUS_INVALID_OBJECT_ID);

        status = lag_api->remove_lag_member(12345);
        assert(status == SAI_STATUS_INVALID_PARAMETER);
    }
}

void test_lag_member_capacity(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test lag member capacity\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id1 = 0;
    sai_object_id_t lag_id2 = 0;

    status = lag_api->create_lag(
        &lag_id1, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->create_lag(
        &lag_id2, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    sai_object_id_t lag_member_id = 0;
    sai_attribute_t attr_list[2];
    attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attr_list[0].value.oid = lag_id1;
    attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    sai_object_id_t* port_id = &attr_list[1].value.oid;

    for (int idx = 0; idx < 16; idx++) {
        *port_id = create_port_object(idx);
        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    *port_id = create_port_object(16);
    status = lag_api->create_lag_member(
        &lag_member_id, 2, attr_list);
    assert(status == SAI_STATUS_TABLE_FULL);
    
    status = lag_api->remove_lag_member(lag_member_id);
    assert(status == SAI_STATUS_SUCCESS);

    attr_list[0].value.oid = lag_id2;
    *port_id = create_port_object(16);
    status = lag_api->create_lag_member(
        &lag_member_id, 2, attr_list);
    assert(status == SAI_STATUS_SUCCESS);

    *port_id = create_port_object(17);
    status = lag_api->create_lag_member(
        &lag_member_id, 2, attr_list);
    assert(status == SAI_STATUS_TABLE_FULL);
}

void test_set_lag_member_attribute(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test set lag member attribute\n");
    printf("\n");

    sai_status_t status = 0;

    sai_attribute_t attr;  
    attr.id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attr.value.oid = 321;

    status = lag_api->set_lag_attribute(123, &attr);
    assert(status == SAI_STATUS_INVALID_ATTRIBUTE_0);
}

void test_get_lag_member_attribute(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test get lag member attribute\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag_id = 0;
    sai_object_id_t lag_member_id = 0;

    status = lag_api->create_lag(
        &lag_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);

        status = lag_api->create_lag_member(
            &lag_member_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    {
        sai_attribute_t attr_list[2];
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;

        status = lag_api->get_lag_member_attribute(
            lag_member_id,
            2,
            attr_list
        );
        assert(status == SAI_STATUS_SUCCESS);

        assert(attr_list[0].value.oid == lag_id);
        assert(attr_list[1].value.oid == create_port_object(0));
    }
}

void test_happy_pass(sai_lag_api_t* lag_api) {
    printf("\n");
    printf("Test happy pass\n");
    printf("\n");

    sai_status_t status = 0;

    sai_object_id_t lag1_id = 0;
    sai_object_id_t lag_member11_id = 0;
    sai_object_id_t lag_member12_id = 0;

    sai_object_id_t lag2_id = 0;
    sai_object_id_t lag_member21_id = 0;
    sai_object_id_t lag_member22_id = 0;

    status = lag_api->create_lag(
        &lag1_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag1_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attr_list[1].value.oid = create_port_object(0);

        status = lag_api->create_lag_member(
            &lag_member11_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag1_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = create_port_object(1);

        status = lag_api->create_lag_member(
            &lag_member12_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    status = lag_api->create_lag(
        &lag2_id, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag2_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = create_port_object(2);

        status = lag_api->create_lag_member(
            &lag_member21_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag2_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = create_port_object(3);

        status = lag_api->create_lag_member(
            &lag_member22_id, 2, attr_list);
        assert(status == SAI_STATUS_SUCCESS);
    }

    {
        sai_attribute_t attr;  
        attr.id = SAI_LAG_ATTR_PORT_LIST;
        
        sai_object_id_t list[16] = {0};
        attr.value.objlist.count = 16;
        attr.value.objlist.list = list;

        status = lag_api->get_lag_attribute(
            lag1_id, 1, &attr);
        assert(status == SAI_STATUS_SUCCESS);

        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(0);
        expected_memory[1] = create_port_object(1);

        sai_object_list_t expected_list;
        expected_list.count = 2;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    {
        sai_attribute_t attr;  
        attr.id = SAI_LAG_ATTR_PORT_LIST;
        
        sai_object_id_t list[16] = {0};
        attr.value.objlist.count = 16;
        attr.value.objlist.list = list;

        status = lag_api->get_lag_attribute(lag2_id, 1, &attr);
        assert(status == SAI_STATUS_SUCCESS);

        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(2);
        expected_memory[1] = create_port_object(3);

        sai_object_list_t expected_list;
        expected_list.count = 2;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;

        status = lag_api->get_lag_member_attribute(
            lag_member11_id, 1, attr_list);
        assert(status == SAI_STATUS_SUCCESS);

        assert(attr_list[0].value.oid == lag1_id);
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;

        status = lag_api->get_lag_member_attribute(
            lag_member21_id, 1, attr_list);
        assert(status == SAI_STATUS_SUCCESS);

        assert(attr_list[0].value.oid == create_port_object(2));
    }

    status = lag_api->remove_lag_member(lag_member12_id);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_attribute_t attr;  
        attr.id = SAI_LAG_ATTR_PORT_LIST;
        
        sai_object_id_t list[16] = {0};
        attr.value.objlist.count = 16;
        attr.value.objlist.list = list;

        status = lag_api->get_lag_attribute(lag1_id, 1, &attr);
        assert(status == SAI_STATUS_SUCCESS);

        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(0);

        sai_object_list_t expected_list;
        expected_list.count = 1;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    status = lag_api->remove_lag_member(lag_member21_id);
    assert(status == SAI_STATUS_SUCCESS);

    {
        sai_attribute_t attr;  
        attr.id = SAI_LAG_ATTR_PORT_LIST;
        
        sai_object_id_t list[16] = {0};
        attr.value.objlist.count = 16;
        attr.value.objlist.list = list;

        status = lag_api->get_lag_attribute(lag2_id, 1, &attr);
        assert(status == SAI_STATUS_SUCCESS);

        sai_object_id_t expected_memory[16] = {0};
        expected_memory[0] = create_port_object(3);

        sai_object_list_t expected_list;
        expected_list.count = 1;
        expected_list.list = expected_memory;

        assert(compare_lists(
            &attr.value.objlist, &expected_list, 16) == 0);
    }

    status = lag_api->remove_lag_member(lag_member11_id);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->remove_lag_member(lag_member22_id);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->remove_lag(lag1_id);
    assert(status == SAI_STATUS_SUCCESS);

    status = lag_api->remove_lag(lag2_id);
    assert(status == SAI_STATUS_SUCCESS);
}

sai_lag_api_t* get_new_lag_api() {
    sai_api_uninitialize();
    sai_lag_api_t *lag_api = NULL;
    sai_status_t status = 0;

    status = sai_api_initialize(0, &test_services);
    assert(status == SAI_STATUS_SUCCESS);

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    assert(status == SAI_STATUS_SUCCESS);

    return lag_api;
}

int main()
{
    sai_lag_api_t* lag_api = NULL;
    
    lag_api = get_new_lag_api();
    test_create_lag(lag_api);

    lag_api = get_new_lag_api();
    test_remove_lag(lag_api);

    lag_api = get_new_lag_api();
    test_lag_capacity(lag_api);
    
    lag_api = get_new_lag_api();
    test_set_lag_attribute(lag_api);

    lag_api = get_new_lag_api();
    test_get_lag_attribute(lag_api);

    lag_api = get_new_lag_api();
    test_create_lag_member(lag_api);

    lag_api = get_new_lag_api();
    test_remove_lag_member(lag_api);

    lag_api = get_new_lag_api();
    test_lag_member_capacity(lag_api);
    
    lag_api = get_new_lag_api();
    test_set_lag_member_attribute(lag_api);

    lag_api = get_new_lag_api();
    test_get_lag_member_attribute(lag_api);

    lag_api = get_new_lag_api();
    test_happy_pass(lag_api);

    return 0;
}
