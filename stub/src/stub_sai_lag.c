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

static sai_status_t stub_lag_attrib_get(
    _In_ const sai_object_key_t   *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t                  attr_index,
    _Inout_ vendor_cache_t        *cache,
    void                          *arg
);

static sai_status_t stub_lag_member_attrib_get(
    _In_ const sai_object_key_t   *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t                  attr_index,
    _Inout_ vendor_cache_t        *cache,
    void                          *arg
);

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST,
      { false, false, false, true },
      { false, false, false, true },
      stub_lag_attrib_get, (void*)SAI_LAG_ATTR_PORT_LIST,
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
      stub_lag_member_attrib_get, (void*)SAI_LAG_MEMBER_ATTR_LAG_ID,
      NULL, NULL },
    { SAI_LAG_MEMBER_ATTR_PORT_ID,
      { true, true, false, true },
      { true, true, false, true },
      stub_lag_member_attrib_get, (void*)SAI_LAG_MEMBER_ATTR_PORT_ID,
      NULL, NULL }
};

#define LAG_COUNT 5
#define LAG_MEMBER_COUNT 16
#define PORT_COUNT 32

typedef struct lag_s lag_t;
typedef struct lag_memeber_s lag_member_t;

struct lag_memeber_s {
    bool taken;
    sai_object_id_t lag;
    sai_object_id_t port;
};

struct lag_s {
    bool taken;
    sai_object_id_t members[LAG_MEMBER_COUNT];
};

typedef struct lag_db_s {
    lag_t lags[LAG_COUNT];
    lag_member_t members[LAG_MEMBER_COUNT];
} lag_db_t;

static lag_db_t LAG_DB;

void db_lag_unitialize() {
    memset((void*)&LAG_DB, 0, sizeof(LAG_DB));
}

static sai_status_t get_lag_entry(sai_object_id_t lag_id, lag_t** entry) {
    assert(entry != NULL);

    sai_status_t status = 0;

    uint32_t lag_idx = 0;

    status = stub_object_to_type(
        lag_id,
        SAI_OBJECT_TYPE_LAG,
        &lag_idx);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    if (lag_idx >= LAG_COUNT) {
        printf("Bad lag(0x%010lX)\n", lag_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    if (!LAG_DB.lags[lag_idx].taken) {
        printf("Bad lag(0x%010lX)\n", lag_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    *entry = &LAG_DB.lags[lag_idx];

    return status;
}

static sai_status_t get_lag_member_entry(
    sai_object_id_t lag_member_id, lag_member_t** entry) {
    assert(entry != NULL);

    sai_status_t status = 0;

    uint32_t lag_member_idx = 0;

    status = stub_object_to_type(
        lag_member_id,
        SAI_OBJECT_TYPE_LAG_MEMBER,
        &lag_member_idx);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    if (lag_member_idx >= LAG_MEMBER_COUNT) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    if (!LAG_DB.members[lag_member_idx].taken) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    *entry = &LAG_DB.members[lag_member_idx];

    return status;
}

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    assert(lag_id != NULL);

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

    uint32_t lag_idx = UINT32_MAX;
    
    for (int idx = 0; idx < LAG_COUNT; idx++) {
        if (!LAG_DB.lags[idx].taken) {
            lag_idx = idx;
            break;
        }
    }

    if (lag_idx == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }

    status = stub_create_object(SAI_OBJECT_TYPE_LAG, lag_idx, lag_id);
    if (status == SAI_STATUS_SUCCESS) {
        LAG_DB.lags[lag_idx].taken = true;
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

    lag_t *entry = NULL;
    sai_status_t status = get_lag_entry(lag_id, &entry);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    if (!entry->taken) {
        printf("LAG(0x%010lX) does not exist\n", lag_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (entry->members[idx] != 0) {
            printf("Failed to delete LAG(0x%010lX) as it has one or more members\n", lag_id);
            return SAI_STATUS_OBJECT_IN_USE;
        }
    }

    entry->taken = false;

    return SAI_STATUS_SUCCESS;
}

static void lag_key_to_str(
    _In_ sai_object_id_t lag_id,
    _Out_ char *key_str)
{
    assert(key_str != NULL);

    uint32_t id;

    if (SAI_STATUS_SUCCESS != stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, &id)) {
        snprintf(key_str, MAX_KEY_STR_LEN, "invalid lag");
    } else {
        snprintf(key_str, MAX_KEY_STR_LEN, "lag %u", id);
    }
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .object_id = lag_id };
    char                   key_str[MAX_KEY_STR_LEN];

    STUB_LOG_ENTER();

    lag_key_to_str(lag_id, key_str);
    return sai_set_attribute(
        &key,
        key_str,
        lag_attribs,
        lag_vendor_attribs,
        attr);
}

sai_status_t stub_lag_attrib_get(
    _In_ const sai_object_key_t   *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t                  attr_index,
    _Inout_ vendor_cache_t        *cache,
    void                          *arg) 
{
    assert(key != NULL);
    assert(value != NULL);

    int64_t attr_type = (int64_t)arg;
    
    assert((SAI_LAG_ATTR_PORT_LIST == attr_type));

    sai_status_t status = 0;

    uint32_t idx = 0;
    status = stub_object_to_type(
        key->object_id,
        SAI_OBJECT_TYPE_LAG,
        &idx
    );
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    if (idx >= LAG_COUNT) {
        printf("Bad lag(0x%010lX)\n", key->object_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    lag_t* lag_entry = &LAG_DB.lags[idx];

    if (!lag_entry->taken) {
        printf("LAG(0x%010lX) does not exist\n", key->object_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    sai_object_list_t* list = &value->objlist;

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (lag_entry->members[idx] != 0) {
            lag_member_t *lag_member_entry = NULL;

            status = get_lag_member_entry(
                lag_entry->members[idx],
                &lag_member_entry);
            // if status is not successful it means that invariants
            // are broken and db is corrupted
            assert(status == SAI_STATUS_SUCCESS);

            list->list[list->count] = lag_member_entry->port;
            list->count += 1;
        }
    }

    return status;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = lag_id };
    char                   key_str[MAX_KEY_STR_LEN];

    STUB_LOG_ENTER();

    lag_key_to_str(lag_id, key_str);
    return sai_get_attributes(
        &key,
        key_str,
        lag_attribs,
        lag_vendor_attribs,
        attr_count,
        attr_list);
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    assert(lag_member_id != NULL);

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
    uint32_t lag_idx = UINT32_MAX;

    status = stub_object_to_type(
        lag_id,
        SAI_OBJECT_TYPE_LAG,
        &lag_idx);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get lag element id, ec: 0x%016X\n", status);
        return status;
    }

    // validate LAG

    if (lag_idx >= LAG_COUNT) {
        printf("Bad LAG: 0x%010lX\n", lag_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (!LAG_DB.lags[lag_idx].taken) {
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
    uint32_t port_idx = UINT32_MAX;

    status = stub_object_to_type(
        port_id,
        SAI_OBJECT_TYPE_PORT,
        &port_idx);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get port element id, ec: 0x%016X\n", status);
        return status;
    }

    // validate port

    if (port_idx >= PORT_COUNT) {
        printf("Bad port %d\n", port_idx);
        return SAI_STATUS_INVALID_PORT_NUMBER;
    }

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (LAG_DB.members[idx].port == port_id) {
            printf("Port(%d) is already a member of lag\n", port_idx);
            return SAI_STATUS_INVALID_PORT_MEMBER;
        }
    }

    // Find db index

    uint32_t lag_member_idx = UINT32_MAX;

    for (int idx = 0; idx < LAG_MEMBER_COUNT; idx++) {
        if (!LAG_DB.members[idx].taken) {
            lag_member_idx = idx;
            break;
        }
    }

    if (lag_member_idx == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }
    
    status = stub_create_object(
        SAI_OBJECT_TYPE_LAG_MEMBER,
        lag_member_idx,
        lag_member_id);
    
    if (status == SAI_STATUS_SUCCESS) {
        lag_t* lag_entry = &LAG_DB.lags[lag_idx];

        lag_member_t* lag_member_entry = 
            &LAG_DB.members[lag_member_idx];

        lag_member_entry->taken = true;
        lag_member_entry->lag = lag_id;
        lag_member_entry->port = port_id;

        lag_entry->members[lag_member_idx] = *lag_member_id;
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

    uint32_t lag_member_idx = UINT32_MAX;

    status = stub_object_to_type(
        lag_member_id,
        SAI_OBJECT_TYPE_LAG_MEMBER,
        &lag_member_idx);
    if (status != SAI_STATUS_SUCCESS) {
        printf(
            "Failed to get lag member element id, ec: 0x%016X\n",
            status);
        return status;
    }

    if (lag_member_idx >= LAG_MEMBER_COUNT) {
        printf("Bad lag member(0x%010lX)\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }
    
    lag_member_t* lag_member_entry = &LAG_DB.members[lag_member_idx];
    if (!lag_member_entry->taken) {
        printf("Lag member(0x%010lX) does not exist\n", lag_member_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    lag_t *lag_entry = NULL;
    status = get_lag_entry(lag_member_entry->lag, &lag_entry);
    // if status is not successful it means that invariants
    // are broken and db is corrupted
    assert(status == SAI_STATUS_SUCCESS);

    sai_object_id_t port_id = lag_member_entry->port;
    uint32_t port_idx = UINT32_MAX;

    status = stub_object_to_type(
        port_id,
        SAI_OBJECT_TYPE_PORT,
        &port_idx);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get port element id, ec: 0x%016X\n", status);
        return status;
    }

    lag_entry->members[lag_member_idx] = 0;

    lag_member_entry->taken = 0;
    lag_member_entry->lag = 0;
    lag_member_entry->port = 0;

    return SAI_STATUS_SUCCESS;
}

static void lag_member_key_to_str(
    _In_ sai_object_id_t lag_member_id,
    _Out_ char *key_str)
{
    assert(key_str != NULL);

    uint32_t id;

    if (SAI_STATUS_SUCCESS != stub_object_to_type(
        lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, &id)) {
        snprintf(key_str, MAX_KEY_STR_LEN, "invalid member lag");
    } else {
        snprintf(key_str, MAX_KEY_STR_LEN, "lag member %u", id);
    }
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .object_id = lag_member_id };
    char                   key_str[MAX_KEY_STR_LEN];

    STUB_LOG_ENTER();

    lag_member_key_to_str(lag_member_id, key_str);
    return sai_set_attribute(
        &key,
        key_str,
        lag_member_attribs,
        lag_member_vendor_attribs,
        attr);
}

sai_status_t stub_lag_member_attrib_get(
    _In_ const sai_object_key_t   *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t                  attr_index,
    _Inout_ vendor_cache_t        *cache,
    void                          *arg
)
{
    assert(key != NULL);
    assert(value != NULL);

    int64_t attr_type = (int64_t)arg;
    
    assert(
        (SAI_LAG_MEMBER_ATTR_LAG_ID == attr_type) ||
        (SAI_LAG_MEMBER_ATTR_PORT_ID == attr_type)
    );

    sai_status_t status = 0;

    uint32_t lag_member_idx = 0;

    status = stub_object_to_type(
        key->object_id,
        SAI_OBJECT_TYPE_LAG_MEMBER,
        &lag_member_idx);
    if (status != SAI_STATUS_SUCCESS) {
        printf(
            "Failed to get lag member element id, ec: 0x%016X\n",
            status);
        return status;
    }

    if (lag_member_idx >= LAG_MEMBER_COUNT) {
        printf("Bad lag member(0x%010lX)\n", key->object_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    lag_member_t* entry = &LAG_DB.members[lag_member_idx];

    switch (attr_type) {
        case SAI_LAG_MEMBER_ATTR_LAG_ID:
            value->oid = entry->lag;
            break;
        case SAI_LAG_MEMBER_ATTR_PORT_ID:
            value->oid = entry->port;
            break;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = lag_member_id };
    char                   key_str[MAX_KEY_STR_LEN];

    STUB_LOG_ENTER();

    lag_member_key_to_str(lag_member_id, key_str);
    return sai_get_attributes(
        &key,
        key_str,
        lag_member_attribs,
        lag_member_vendor_attribs,
        attr_count,
        attr_list);
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
