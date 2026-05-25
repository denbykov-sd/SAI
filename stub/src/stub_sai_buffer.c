#include "stub_sai.h"

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_POOL_COUNT 4
#define BUFFER_PROFILE_COUNT 8

#define SAI_RETURN_ON_ERROR(expr)          \
    do {                                   \
        sai_status_t _status = (expr);     \
        if (_status != SAI_STATUS_SUCCESS) { \
            return _status;                \
        }                                  \
    } while (0)

static const sai_attribute_entry_t buffer_pool_attribs[] = {
    {
        SAI_BUFFER_POOL_ATTR_SHARED_SIZE, 
        false, false, false, true,
        "Shared Buffer Size",
        SAI_ATTR_VAL_TYPE_U32 
    },
    {
        SAI_BUFFER_POOL_ATTR_TYPE, 
        true, true, false, true,
        "Buffer Pool Type",
        SAI_ATTR_VAL_TYPE_S32
    },
    {
        SAI_BUFFER_POOL_ATTR_SIZE, 
        true, true, true, true,
        "Buffer Pool Size",
        SAI_ATTR_VAL_TYPE_U32
    },
    {
        SAI_BUFFER_POOL_ATTR_TH_MODE, 
        false, true, false, true,
        "Threshold Mode for the Buffer Pool",
        SAI_ATTR_VAL_TYPE_S32
    },

    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static sai_status_t stub_buffer_pool_get(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg);

static const sai_vendor_attribute_entry_t buffer_pool_vendor_attribs[] = {
    {
        SAI_BUFFER_POOL_ATTR_SHARED_SIZE,
      { false, false, false, false },
      { false, false, false, false },
      stub_buffer_pool_get, (void*)SAI_BUFFER_POOL_ATTR_SHARED_SIZE,
      NULL, NULL
    },
    {
        SAI_BUFFER_POOL_ATTR_TYPE,
      { true, true, false, true },
      { true, true, false, true },
      stub_buffer_pool_get, (void*)SAI_BUFFER_POOL_ATTR_TYPE,
      NULL, NULL
    },
    {
        SAI_BUFFER_POOL_ATTR_SIZE,
      { true, true, true, true },
      { true, true, true, true },
      stub_buffer_pool_get, (void*)SAI_BUFFER_POOL_ATTR_SIZE,
      NULL, NULL
    },
    {
        SAI_BUFFER_POOL_ATTR_TH_MODE,
      { true, true, false, true },
      { true, true, false, true },
      stub_buffer_pool_get, (void*)SAI_BUFFER_POOL_ATTR_TH_MODE,
      NULL, NULL
    },
};

static const sai_attribute_entry_t buffer_profile_attribs[] = {
    {
        SAI_BUFFER_PROFILE_ATTR_POOL_ID, 
        true, true, true, true,
        "Buffer Pool ID",
        SAI_ATTR_VAL_TYPE_OID
    },
    {
        SAI_BUFFER_PROFILE_ATTR_BUFFER_SIZE, 
        true, true, true, true,
        "Reserved Buffer Size",
        SAI_ATTR_VAL_TYPE_U32
    },
    {
        SAI_BUFFER_PROFILE_ATTR_SHARED_DYNAMIC_TH, 
        false, true, true, true,
        "Dynamic Buffer Threshold",
        SAI_ATTR_VAL_TYPE_S8
    },
    {
        SAI_BUFFER_PROFILE_ATTR_SHARED_STATIC_TH, 
        false, true, true, true,
        "Static Buffer Threshold",
        SAI_ATTR_VAL_TYPE_U32
    },
    {
        SAI_BUFFER_PROFILE_ATTR_XOFF_TH, 
        false, true, true, true,
        "XOFF ingress Buffer Threshold",
        SAI_ATTR_VAL_TYPE_U32
    },
    {
        SAI_BUFFER_PROFILE_ATTR_XON_TH, 
        false, true, true, true,
        "XON ingress Buffer Threshold",
        SAI_ATTR_VAL_TYPE_U32
    },

    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static sai_status_t stub_buffer_profile_get(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg);

static const sai_vendor_attribute_entry_t buffer_profile_vendor_attribs[] = {
    {
        SAI_BUFFER_PROFILE_ATTR_POOL_ID,
      { true, true, true, true },
      { true, true, true, true },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_POOL_ID,
      NULL, NULL
    },
    {
        SAI_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
      { true, true, true, true },
      { true, true, true, true },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
      NULL, NULL
    },
    {
        SAI_BUFFER_PROFILE_ATTR_SHARED_DYNAMIC_TH,
      { false, true, true, true },
      { false, true, true, true },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_SHARED_DYNAMIC_TH,
      NULL, NULL
    },
    {
        SAI_BUFFER_PROFILE_ATTR_SHARED_STATIC_TH,
      { false, true, true, true },
      { false, true, true, true },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_SHARED_STATIC_TH,
      NULL, NULL
    },
    {
        SAI_BUFFER_PROFILE_ATTR_XOFF_TH,
      { false, false, false, false },
      { false, false, false, false },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_XOFF_TH,
      NULL, NULL
    },
    {
        SAI_BUFFER_PROFILE_ATTR_XON_TH,
      { false, false, false, false },
      { false, false, false, false },
      stub_buffer_profile_get, (void*)SAI_BUFFER_PROFILE_ATTR_XON_TH,
      NULL, NULL
    }
};

typedef struct buffer_pool_entry_s {
    bool taken;
    sai_buffer_pool_type_t type;
    sai_uint32_t size;
    sai_buffer_threshold_mode_t th_mode;
} buffer_pool_entry_t;

typedef struct buffer_profile_entry_s {
    bool taken;
    sai_object_id_t pool_id;
    sai_uint32_t buffer_size;
    sai_int8_t dynamic_th;
    sai_uint32_t static_th;
} buffer_profile_entry_t;

typedef struct buffer_db_s {
    buffer_pool_entry_t pools[BUFFER_POOL_COUNT];
    buffer_profile_entry_t profiles[BUFFER_PROFILE_COUNT];
} buffer_db_t;

static buffer_db_t DB = {0};

void buffer_db_uninitialize() {
    memset((void*)&DB, 0, sizeof(DB));
}

static sai_status_t get_buffer_pool_entry(
    sai_object_id_t pool_id, buffer_pool_entry_t** entry) {
    assert(entry != NULL);

    uint32_t pool_idx = 0;

    SAI_RETURN_ON_ERROR(
        stub_object_to_type(
        pool_id,
        SAI_OBJECT_TYPE_BUFFER_POOL,
        &pool_idx)
    );

    if (pool_idx >= BUFFER_POOL_COUNT) {
        printf("Bad pool(0x%010lX)\n", pool_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    if (!DB.pools[pool_idx].taken) {
        printf("Bad pool(0x%010lX)\n", pool_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    *entry = &DB.pools[pool_idx];

    return SAI_STATUS_SUCCESS;
}

static sai_status_t get_buffer_profile_entry(
    sai_object_id_t profile_id, buffer_profile_entry_t** entry) {
    assert(entry != NULL);

    uint32_t profile_idx = 0;

    SAI_RETURN_ON_ERROR(
        stub_object_to_type(
        profile_id,
        SAI_OBJECT_TYPE_BUFFER_PROFILE,
        &profile_idx)
    );

    if (profile_idx >= BUFFER_POOL_COUNT) {
        printf("Bad profile(0x%010lX)\n", profile_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    if (!DB.profiles[profile_idx].taken) {
        printf("Bad profile(0x%010lX)\n", profile_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    *entry = &DB.profiles[profile_idx];

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_buffer_pool(
    _Out_ sai_object_id_t* pool_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list)
{
    assert(pool_id != NULL);

    sai_status_t status = 0;

    // extract attributes

    sai_buffer_pool_type_t type = 0;
    sai_uint32_t size = 0;
    sai_buffer_threshold_mode_t th_mode = SAI_BUFFER_THRESHOLD_MODE_DYNAMIC;

    uint32_t type_idx = 0;
    uint32_t size_idx = 0;
    uint32_t th_mode_idx = 0;

    status = check_attribs_metadata(
        attr_count,
        attr_list,
        buffer_pool_attribs,
        buffer_pool_vendor_attribs,
        SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    const sai_attribute_value_t *attr_value;

    status = find_attrib_in_list(
        attr_count, attr_list,
        SAI_BUFFER_POOL_ATTR_TYPE,
        &attr_value,
        &type_idx);
    assert(status == SAI_STATUS_SUCCESS);
    type = attr_value->s32;

    status = find_attrib_in_list(
        attr_count, attr_list,
        SAI_BUFFER_POOL_ATTR_SIZE,
        &attr_value,
        &size_idx);
    assert(status == SAI_STATUS_SUCCESS);
    size = attr_value->u32;

    status = find_attrib_in_list(
        attr_count, attr_list,
        SAI_BUFFER_POOL_ATTR_TH_MODE,
        &attr_value,
        &th_mode_idx);
    if(status == SAI_STATUS_SUCCESS) {
        th_mode = attr_value->s32;
    }

    // validate attributes
    if ((type != SAI_BUFFER_POOL_INGRESS) && 
        (type != SAI_BUFFER_POOL_EGRESS)) {
        return SAI_STATUS_INVALID_ATTRIBUTE_0 + type_idx;
    }

    if ((th_mode != SAI_BUFFER_THRESHOLD_MODE_STATIC) && 
        (th_mode != SAI_BUFFER_THRESHOLD_MODE_DYNAMIC)) {
        return SAI_STATUS_INVALID_ATTRIBUTE_0 + th_mode_idx;
    }

    // create object

    uint32_t pool_idx = UINT32_MAX;
    
    for (int idx = 0; idx < BUFFER_POOL_COUNT; idx++) {
        if (!DB.pools[idx].taken) {
            pool_idx = idx;
            break;
        }
    }

    if (pool_idx == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }

    status = stub_create_object(SAI_OBJECT_TYPE_BUFFER_POOL, pool_idx, pool_id);
    if (status == SAI_STATUS_SUCCESS) {
        buffer_pool_entry_t* entry = &DB.pools[pool_idx];
        entry->taken = true;
        entry->type = type;
        entry->size = size;
        entry->th_mode = th_mode;
    } else {
        printf("Cannot create Buffer Pool\n");
    }

    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    status = sai_attr_list_to_str(
        attr_count,
        attr_list,
        buffer_pool_attribs,
        MAX_LIST_VALUE_STR_LEN,
        list_str);
    assert(status == SAI_STATUS_SUCCESS);
    
    printf("Create Buffer Pool: 0x%010lX (%s)\n", (uint64_t)*pool_id, list_str);

    return status;
}

sai_status_t stub_remove_buffer_pool(
    _In_ sai_object_id_t  pool_id)
{
    buffer_pool_entry_t *entry = NULL;
    
    SAI_RETURN_ON_ERROR(get_buffer_pool_entry(pool_id, &entry));

    if (!entry->taken) {
        printf("Buffer pool(0x%010lX) does not exist\n", pool_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    entry->taken = false;

    printf("Remove Buffer Pool: 0x%010lX\n", pool_id);

    return SAI_STATUS_SUCCESS;
}

static void buffer_pool_key_to_str(
    _In_ sai_object_id_t pool_id,
    _Out_ char *key_str)
{
    assert(key_str != NULL);

    uint32_t id;

    if (SAI_STATUS_SUCCESS != stub_object_to_type(
        pool_id,
        SAI_OBJECT_TYPE_BUFFER_POOL,
        &id)) {
        snprintf(key_str, MAX_KEY_STR_LEN, "invalid buffer pool");
    } else {
        snprintf(key_str, MAX_KEY_STR_LEN, "buffer pool %u", id);
    }
}

sai_status_t stub_set_buffer_pool_attribute(
    _In_ sai_object_id_t pool_id,
    _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .object_id = pool_id };
    char                   key_str[MAX_KEY_STR_LEN];

    buffer_pool_key_to_str(pool_id, key_str);
    return sai_set_attribute(
        &key,
        key_str,
        buffer_pool_attribs,
        buffer_pool_vendor_attribs,
        attr);
}

sai_status_t stub_get_buffer_pool_attribute(
    _In_ sai_object_id_t pool_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = pool_id };
    char                   key_str[MAX_KEY_STR_LEN];

    buffer_pool_key_to_str(pool_id, key_str);
    return sai_get_attributes(
        &key,
        key_str,
        buffer_pool_attribs,
        buffer_pool_vendor_attribs,
        attr_count,
        attr_list);
}

static sai_status_t stub_buffer_pool_get(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg) {
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_create_buffer_profile(
    _Out_ sai_object_id_t* profile_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list)
{
    assert(profile_id != NULL);

    sai_status_t status = 0;

    status = check_attribs_metadata(
        attr_count,
        attr_list,
        buffer_profile_attribs,
        buffer_profile_vendor_attribs,
        SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Attribute check failed, ec: 0x%016X\n", status);
        return status;
    }

    uint32_t profile_idx = UINT32_MAX;
    
    for (int idx = 0; idx < BUFFER_PROFILE_COUNT; idx++) {
        if (!DB.profiles[idx].taken) {
            profile_idx = idx;
            break;
        }
    }

    if (profile_idx == UINT32_MAX) {
        return SAI_STATUS_TABLE_FULL;
    }

    status = stub_create_object(SAI_OBJECT_TYPE_BUFFER_POOL, profile_idx, profile_id);
    if (status == SAI_STATUS_SUCCESS) {
        DB.profiles[profile_idx].taken = true;
    } else {
        printf("Cannot create Buffer Pool\n");
    }

    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    status = sai_attr_list_to_str(
        attr_count,
        attr_list,
        buffer_profile_attribs,
        MAX_LIST_VALUE_STR_LEN,
        list_str);
    assert(status == SAI_STATUS_SUCCESS);
    
    printf("Create Buffer Profile: 0x%010lX (%s)\n", (uint64_t)*profile_id, list_str);

    return status;
}

sai_status_t stub_remove_buffer_profile(
    _In_ sai_object_id_t  profile_id)
{
    buffer_profile_entry_t *entry = NULL;
    
    SAI_RETURN_ON_ERROR(get_buffer_profile_entry(profile_id, &entry));

    if (!entry->taken) {
        printf("Buffer profile(0x%010lX) does not exist\n", profile_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    entry->taken = false;

    printf("Remove Buffer Profile: 0x%010lX\n", profile_id);

    return SAI_STATUS_SUCCESS;
}

static void buffer_profile_key_to_str(
    _In_ sai_object_id_t pool_id,
    _Out_ char *key_str)
{
    assert(key_str != NULL);

    uint32_t id;

    if (SAI_STATUS_SUCCESS != stub_object_to_type(
        pool_id,
        SAI_OBJECT_TYPE_BUFFER_POOL,
        &id)) {
        snprintf(key_str, MAX_KEY_STR_LEN, "invalid buffer pool");
    } else {
        snprintf(key_str, MAX_KEY_STR_LEN, "buffer pool %u", id);
    }
}

sai_status_t stub_set_buffer_profile_attribute(
    _In_ sai_object_id_t profile_id,
    _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .object_id = profile_id };
    char                   key_str[MAX_KEY_STR_LEN];

    buffer_profile_key_to_str(profile_id, key_str);
    return sai_set_attribute(
        &key,
        key_str,
        buffer_profile_attribs,
        buffer_profile_vendor_attribs,
        attr);
}

sai_status_t stub_get_buffer_profile_attribute(
    _In_ sai_object_id_t profile_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = profile_id };
    char                   key_str[MAX_KEY_STR_LEN];

    buffer_profile_key_to_str(profile_id, key_str);
    return sai_get_attributes(
        &key,
        key_str,
        buffer_profile_attribs,
        buffer_profile_vendor_attribs,
        attr_count,
        attr_list);
}

static sai_status_t stub_buffer_profile_get(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg) {
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_get_buffer_pool_stats(
    _In_ sai_object_id_t pool_id,
    _In_ const sai_buffer_pool_stat_counter_t *counter_ids,
    _In_ uint32_t number_of_counters,
    _Out_ uint64_t* counters)
{
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_set_ingress_priority_group_attribute(
    _In_ sai_object_id_t ingress_pg_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_get_ingress_priority_group_attribute(
    _In_ sai_object_id_t ingress_pg_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list) 
{
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_get_ingress_priority_group_stats(
    _In_ sai_object_id_t ingress_pg_id,
    _In_ const sai_ingress_priority_group_stat_counter_t *counter_ids,
    _In_ uint32_t number_of_counters,
    _Out_ uint64_t* counters)
{
    return SAI_STATUS_NOT_SUPPORTED;
}

sai_status_t stub_clear_ingress_priority_group_stats(
    _In_ sai_object_id_t ingress_pg_id,
    _In_ const sai_ingress_priority_group_stat_counter_t *counter_ids,
    _In_ uint32_t number_of_counters)
{
    return SAI_STATUS_NOT_SUPPORTED;
}

const sai_buffer_api_t buffer_api =
{
    stub_create_buffer_pool,
    stub_remove_buffer_pool,
    stub_set_buffer_pool_attribute,
    stub_get_buffer_pool_attribute,
    stub_get_buffer_pool_stats,
    stub_set_ingress_priority_group_attribute,
    stub_get_ingress_priority_group_attribute,
    stub_get_ingress_priority_group_stats,
    stub_clear_ingress_priority_group_stats,
    stub_create_buffer_profile,
    stub_remove_buffer_profile,
    stub_set_buffer_profile_attribute,
    stub_get_buffer_profile_attribute
};
