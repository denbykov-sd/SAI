#include "stub_sai.h"
#include "assert.h"

#include <stdbool.h>
#include <string.h>
#include <stdint.h>


static const sai_attribute_entry_t lag_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST, false, false, false, true,
      "List of ports in LAG", SAI_ATTR_VAL_TYPE_OBJLIST },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST,
      { false, false, false, true },
      { false, false, false, true },
      NULL, NULL,
      NULL, NULL }
};

static const sai_attribute_entry_t lag_member_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID, 
        true, true, false, true,
      "LAG ID", SAI_ATTR_VAL_TYPE_OID },
    { SAI_LAG_MEMBER_ATTR_PORT_ID, true, true, false, true,
      "PORT ID", SAI_ATTR_VAL_TYPE_OID },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_vendor_attribute_entry_t lag_member_vendor_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID,
      { true, true, false, true },
      { true, true, false, true },
      NULL, NULL,
      NULL, NULL },
    { SAI_LAG_MEMBER_ATTR_PORT_ID,
      { true, true, false, true },
      { true, true, false, true },
      NULL, NULL,
      NULL, NULL }
};

#define LAG_COUNT 5
#define LAG_MEMBER_COUNT 16
#define PORT_COUNT 32

typedef struct lag_memeber_s {
    sai_object_id_t lag;
    sai_object_id_t port;
} lag_member_t;

typedef struct lag_s {
    bool taken;
    lag_member_t members[LAG_MEMBER_COUNT];
} lag_t;

typedef struct lag_db_s {
    lag_t lags[LAG_COUNT];
    lag_t* port_lag[PORT_COUNT];
    lag_t* member_lag[LAG_MEMBER_COUNT];
} lag_db_t;

static lag_db_t LAG_DB;

void db_lag_unitialize() {
    memset((void*)&LAG_DB, 0, sizeof(LAG_DB));
}

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    sai_status_t status = 0;

    status = check_attribs_metadata(
        attr_count,
        attr_list,
        lag_attribs,
        lag_vendor_attribs,
        SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    uint32_t element_id = UINT32_MAX;
    
    for (int idx = 0; idx < LAG_COUNT; idx++) {
        if (!LAG_DB.lags[idx].taken) {
            element_id = idx;
            break;
        }
    }

    if (element_id == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }

    status = stub_create_object(SAI_OBJECT_TYPE_LAG, element_id, lag_id);
    if (status == SAI_STATUS_SUCCESS) {
        LAG_DB.lags[element_id].taken = true;
    } else {
        printf("Cannot create LAG\n");
    }

    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("CREATE LAG: 0x%010lX (%s)\n", (uint64_t)*lag_id, list_str);

    return status;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    printf("REMOVE LAG: 0x%010lX\n", lag_id);

    uint32_t element_id;
    sai_status_t status = stub_object_to_type(
        lag_id,
        SAI_OBJECT_TYPE_LAG,
        &element_id
    );

    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    lag_t* lag_entry = &LAG_DB.lags[element_id];

    if (!lag_entry->taken) {
        printf("LAG(0x%010lX) does not exist\n", lag_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (LAG_DB.member_lag[idx] == lag_entry) {
            printf("Failed to delete LAG(0x%010lX) as it has one or more members\n", lag_id);
            return SAI_STATUS_OBJECT_IN_USE;
        }
    }

    lag_entry->taken = false;

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    printf("SET LAG ATTRIBUTE FOR LAG: 0x%010lX\n", lag_id);
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    assert(attr_list != NULL);

    printf("GET LAG ATTRIBUTE FOR LAG: 0x%010lX\n", lag_id);

    sai_status_t status = check_attribs_metadata(
        attr_count,
        attr_list,
        lag_attribs,
        lag_vendor_attribs,
        SAI_OPERATION_GET);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    uint32_t element_id;
    status = stub_object_to_type(
        lag_id,
        SAI_OBJECT_TYPE_LAG,
        &element_id
    );
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    lag_t* lag_entry = &LAG_DB.lags[element_id];

    if (!lag_entry->taken) {
        printf("LAG(0x%010lX) does not exist\n", lag_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    sai_object_list_t* list = &attr_list->value.objlist;

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (lag_entry->members[idx].lag != 0) {
            list->list[list->count] = lag_entry->members[idx].port;
            list->count += 1;
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    assert(attr_list != NULL);

    sai_status_t status = 0;

    status = check_attribs_metadata(
        attr_count,
        attr_list,
        lag_member_attribs,
        lag_member_vendor_attribs,
        SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    // get LAG

    const sai_attribute_value_t *attr_value;
    uint32_t discard = 0;

    status = find_attrib_in_list(
        attr_count, attr_list,
        SAI_LAG_MEMBER_ATTR_LAG_ID,
        &attr_value,
        &discard);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get attribute, ec: 0x%016X\n", status);
        return status;
    }

    sai_object_id_t lag_id = attr_value->oid;
    uint32_t lag_element_id = UINT32_MAX;

    status = stub_object_to_type(
        lag_id,
        SAI_OBJECT_TYPE_LAG,
        &lag_element_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get lag element id, ec: 0x%016X\n", status);
        return status;
    }

    // validate LAG

    if (lag_element_id >= LAG_COUNT) {
        printf("Bad LAG: 0x%010lX\n", lag_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (!LAG_DB.lags[lag_element_id].taken) {
        printf("LAG(0x%010lX) does not exist!\n", lag_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    // get port

    attr_value = NULL;

    status = find_attrib_in_list(
        attr_count, attr_list,
        SAI_LAG_MEMBER_ATTR_PORT_ID,
        &attr_value,
        &discard);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get attribute, ec: 0x%016X\n", status);
        return status;
    }

    sai_object_id_t port_id = attr_value->oid;
    uint32_t port_element_id = UINT32_MAX;

    status = stub_object_to_type(
        port_id,
        SAI_OBJECT_TYPE_PORT,
        &port_element_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get port element id, ec: 0x%016X\n", status);
        return status;
    }

    // validate port

    if (port_element_id >= PORT_COUNT) {
        printf("Bad port %d\n", port_element_id);
        return SAI_STATUS_INVALID_PORT_NUMBER;
    }

    if (LAG_DB.port_lag[port_element_id] != NULL) {
        printf("Port(%d) is already a member of lag\n", port_element_id);
        return SAI_STATUS_INVALID_PORT_MEMBER;
    }

    // Find db index

    uint32_t lag_member_element_id = UINT32_MAX;

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (LAG_DB.member_lag[idx] == NULL) {
            lag_member_element_id = idx;
            break;
        }
    }

    if (lag_member_element_id == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }
    
    status = stub_create_object(
        SAI_OBJECT_TYPE_LAG_MEMBER,
        lag_member_element_id,
        lag_member_id);
    
    if (status == SAI_STATUS_SUCCESS) {
        lag_t* lag_db_entry = &LAG_DB.lags[lag_element_id];

        lag_member_t* lag_member_db_entry = 
            &lag_db_entry->members[lag_member_element_id];
        lag_member_db_entry->lag = lag_id;
        lag_member_db_entry->port = port_id;

        LAG_DB.member_lag[lag_member_element_id] = lag_db_entry;
        LAG_DB.port_lag[port_element_id] = lag_db_entry;
    } else {
        printf("Cannot create LAG MEMBER\n");
    }

    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("CREATE LAG MEMBER: 0x%010lX (%s)\n", (uint64_t)*lag_member_id, list_str);
    return status;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
    printf("REMOVE LAG MEMBER: 0x%010lX\n", lag_member_id);

    sai_status_t status = 0;

    uint32_t lag_member_element_id = UINT32_MAX;

    status = stub_object_to_type(
        lag_member_id,
        SAI_OBJECT_TYPE_LAG_MEMBER,
        &lag_member_element_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf(
            "Failed to get lag member element id, ec: 0x%016X\n",
            status);
        return status;
    }

    if (lag_member_element_id >= LAG_MEMBER_COUNT) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }
    
    lag_t* lag = LAG_DB.member_lag[lag_member_element_id];
    if (lag == NULL) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    sai_object_id_t port_id = lag->members[lag_member_element_id].port;
    uint32_t port_element_id = UINT32_MAX;

    status = stub_object_to_type(
        port_id,
        SAI_OBJECT_TYPE_PORT,
        &port_element_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get port element id, ec: 0x%016X\n", status);
        return status;
    }

    lag->members[lag_member_element_id].lag = 0;
    lag->members[lag_member_element_id].port = 0;

    LAG_DB.member_lag[lag_member_element_id] = NULL;
    LAG_DB.port_lag[port_element_id] = NULL;

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    printf("SET LAG MEMBER ATTRIBUTE FOR LAG: 0x%010lX\n", lag_member_id);
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    assert(attr_list != NULL);

    printf("GET LAG MEMBER ATTRIBUTE FOR LAG: 0x%010lX\n", lag_member_id);

    sai_status_t status = check_attribs_metadata(
        attr_count,
        attr_list,
        lag_member_attribs,
        lag_member_vendor_attribs,
        SAI_OPERATION_GET);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    uint32_t lag_member_element_id = 0;

    status = stub_object_to_type(
        lag_member_id,
        SAI_OBJECT_TYPE_LAG_MEMBER,
        &lag_member_element_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf(
            "Failed to get lag member element id, ec: 0x%016X\n",
            status);
        return status;
    }

    if (lag_member_element_id >= LAG_MEMBER_COUNT) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    lag_t* lag_entry = LAG_DB.member_lag[lag_member_element_id];
    lag_member_t* entry = lag_entry->members + lag_member_element_id;

    for (uint32_t idx = 0; idx < attr_count; idx++) {
        sai_attribute_t* attr = attr_list + idx;

        switch (attr->id) {
            case SAI_LAG_MEMBER_ATTR_LAG_ID:
                attr->value.oid = entry->lag;
                break;
            case SAI_LAG_MEMBER_ATTR_PORT_ID:
                attr->value.oid = entry->port;
                break;
            default:
                return SAI_STATUS_NOT_IMPLEMENTED;
        }
    }

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
