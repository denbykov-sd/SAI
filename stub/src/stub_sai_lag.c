#include "sai.h"
#include "stub_sai.h"
#include "assert.h"

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    static int32_t next_id = 1;
    sai_status_t status = 0;
    
    status = stub_create_object(SAI_OBJECT_TYPE_LAG, next_id++, lag_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG\n");
    }

    printf("CREATE LAG: 0x%lX\n", (uint64_t)*lag_id);

    return status;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    printf("REMOVE LAG: 0x%lX\n", lag_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    printf("SET LAG ATTRIBUTE FOR LAG: 0x%lX\n", lag_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    printf("GET LAG ATTRIBUTE FOR LAG: 0x%lX\n", lag_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    static int32_t next_id = 1;
    sai_status_t status = 0;
    
    status = stub_create_object(
        SAI_OBJECT_TYPE_LAG_MEMBER,
        next_id++,
        lag_member_id);
    
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG MEMBER\n");
    }

    printf("CREATE LAG MEMBER: 0x%lX", (uint64_t)*lag_member_id);

    for (uint32_t i = 0; i < attr_count; i++) {
        switch (attr_list[i].id) {
            case SAI_LAG_MEMBER_ATTR_LAG_ID:
                printf(" LAG ID: 0x%lX", attr_list[i].value.oid);
                break;
            case SAI_LAG_MEMBER_ATTR_PORT_ID:
                printf(" Port ID: 0x%lX", attr_list[i].value.oid);
                break;
        }
    }

    putc('\n', stdout);

    return status;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
    printf("REMOVE LAG MEMBER: 0x%lX\n", lag_member_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    printf("SET LAG MEMBER ATTRIBUTE FOR LAG: 0x%lX\n", lag_member_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    printf("GET LAG MEMBER ATTRIBUTE FOR LAG: 0x%lX\n", lag_member_id);
    return SAI_STATUS_SUCCESS;
}

const sai_lag_api_t lag_api = {
    stub_create_lag,
    stub_remove_lag,
    stub_set_lag_attribute,
    stub_get_lag_attribute,
    stub_create_lag_member,
    stub_remove_lag_member,
    stub_set_lag_member_attribute,
    stub_get_lag_member_attribute
};
