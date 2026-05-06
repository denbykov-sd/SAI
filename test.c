#include <stdio.h>
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

int main()
{
    sai_status_t status = 0;
    sai_lag_api_t *lag_api = NULL;
    
    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to initialize SAI API, ec: %d\n", status);
        return 1;
    }

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get LAG API, ec: %d\n", status);
        return 1;
    }

    sai_object_id_t lag1_id = 0;
    sai_object_id_t lag_member11_id = 0;
    sai_object_id_t lag_member12_id = 0;

    sai_object_id_t lag2_id = 0;
    sai_object_id_t lag_member21_id = 0;
    sai_object_id_t lag_member22_id = 0;

    status = lag_api->create_lag(
        &lag1_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG, ec: %d\n", status);
        return 1;
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag1_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = 1;

        status = lag_api->create_lag_member(
            &lag_member11_id, 2, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to create LAG member, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag1_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = 2;

        status = lag_api->create_lag_member(
            &lag_member12_id, 2, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to create LAG member, ec: %d\n", status);
            return 1;
        }
    }

    status = lag_api->create_lag(
        &lag2_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG, ec: %d\n", status);
        return 1;
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag2_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = 1;

        status = lag_api->create_lag_member(
            &lag_member21_id, 2, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to create LAG member, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[2];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;  
        attr_list[0].value.oid = lag2_id; 
        attr_list[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;  
        attr_list[1].value.oid = 2;

        status = lag_api->create_lag_member(
            &lag_member22_id, 2, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to create LAG member, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_ATTR_PORT_LIST;
        // attr_list[0].value.objlist = allocate list;

        status = lag_api->get_lag_attribute(lag1_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG port list, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_ATTR_PORT_LIST;
        // attr_list[0].value.objlist = allocate list;

        status = lag_api->get_lag_attribute(lag2_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG port list, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;

        status = lag_api->get_lag_member_attribute(
            lag_member11_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG member LAG id, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;

        status = lag_api->get_lag_member_attribute(
            lag_member21_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG member port id, ec: %d\n", status);
            return 1;
        }
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_ATTR_PORT_LIST;
        // attr_list[0].value.objlist = allocate list;

        status = lag_api->get_lag_attribute(lag1_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG port list, ec: %d\n", status);
            return 1;
        }
    }

    status = lag_api->remove_lag_member(lag_member12_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, ec: %d\n", status);
        return 1;
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_ATTR_PORT_LIST;
        // attr_list[0].value.objlist = allocate list;

        status = lag_api->get_lag_attribute(lag1_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG port list, ec: %d\n", status);
            return 1;
        }
    }

    status = lag_api->remove_lag_member(lag_member21_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, ec: %d\n", status);
        return 1;
    }

    {
        sai_attribute_t attr_list[1];  
        attr_list[0].id = SAI_LAG_ATTR_PORT_LIST;
        // attr_list[0].value.objlist = allocate list;

        status = lag_api->get_lag_attribute(lag2_id, 1, attr_list);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get LAG port list, ec: %d\n", status);
            return 1;
        }
    }

    status = lag_api->remove_lag_member(lag_member11_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, ec: %d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lag_member22_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, ec: %d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag1_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG, ec: %d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag2_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG, ec: %d\n", status);
        return 1;
    }

    sai_api_uninitialize();

    return 0;
}
