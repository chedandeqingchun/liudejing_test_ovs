/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
* Junior University
* Copyright (c) 2011, 2012 Open Networking Foundation
*
* We are making the OpenFlow specification and associated documentation
* (Software) available for public use and benefit with the expectation
* that others will use, modify and enhance the Software and contribute
* those enhancements back to the community. However, since we would
* like to make the Software available for broadest use, with as few
* restrictions as possible permission is hereby granted, free of
* charge, to any person obtaining a copy of this Software to deal in
* the Software under the copyrights without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* The name and trademarks of copyright holder(s) may NOT be used in
* advertising or publicity pertaining to the Software or any
* derivatives without specific, written prior permission.
*/

/* OpenFlow: protocol between controller and datapath. */

#ifndef OPENFLOW_13_H
#define OPENFLOW_13_H 1

#include <openflow/openflow-1.2.h>

/*
 * OpenFlow 1.3 modifies the syntax of the following message types:
 *
 * OFPT_FEATURES_REPLY     = 6    (opf13_switch_features)
 *                                 - new field: auxiliary_id
 *                                 - removed: ofp_ports at the end
 *
 * OFPT_PACKET_IN          = 10   (appends an ovs_be64 to ofp12_packet_in)
 *
 * OpenFlow 1.3 adds following new message types:
 *
 * * Asynchronous message configuration. *
 * OFPT13_GET_ASYNC_REQUEST  = 26   (void)
 * OFPT13_GET_ASYNC_REPLY    = 27   (ofp13_async_config)
 * OFPT13_SET_ASYNC          = 28   (ofp13_async_config)
 *
 * * Meters and rate limiters configuration messages. *
 * OFPT13_METER_MOD          = 29   (ofp13_meter_mod)
 *
 * OpenFlow 1.3 modifies the syntax of the following statistics message types
 * (now called multipart message types):
 *
 * OFPMP13_FLOW_REPLY = 1 (struct ofp13_flow_stats[])
 * OFPMP13_TABLE_REPLY = 3 (struct ofp13_table_stats[])
 * OFPMP13_PORT_REPLY = 4 (struct ofp13_port_stats[])
 * OFPMP13_QUEUE_REPLY = 5, (struct ofp13_queue_stats[])
 * OFPMP13_GROUP_REPLY = 6, (struct ofp13_group_stats[])
 *
 * OpenFlow 1.3 adds the following multipart message types
 *
 * Meter statistics:
 * OFPMP13_METER_REQUEST = 9, (struct ofp13_meter_multipart_request)
 * OFPMP13_METER_REPLY = 9, (struct ofp13_meter_stats[])
 *
 * Meter configuration:
 * OFPMP13_METER_CONFIG_REQUEST = 10, (struct ofp13_meter_multipart_request)
 * OFPMP13_METER_CONFIG_REPLY = 10, (struct ofp13_meter_config[])
 *
 * Meter features:
 * OFPMP13_METER_FEATURES_REQUEST = 11 (void)
 * OFPMP13_METER_FEATURES_REPLY = 11 (struct ofp13_meter_features)
 *
 * Table features:
 * OFPMP13_TABLE_FEATURES_REQUEST = 12, (struct ofp13_table_features[])
 * OFPMP13_TABLE_FEATURES_REPLY = 12, (struct ofp13_table_features[])
 *
 */

enum ofp13_instruction_type {
    OFPIT13_METER = 6           /* Apply meter (rate limiter) */
};

/* Instruction structure for OFPIT_METER */
struct ofp13_instruction_meter {
    ovs_be16 type;              /* OFPIT13_METER */
    ovs_be16 len;               /* Length is 8. */
    ovs_be32 meter_id;          /* Meter instance. */
};
OFP_ASSERT(sizeof(struct ofp13_instruction_meter) == 8);

/* enum ofp_config_flags value OFPC_INVALID_TTL_TO_CONTROLLER
 * is deprecated in OpenFlow 1.3 */

/* Flags to configure the table. Reserved for future use. */
enum ofp13_table_config {
    OFPTC13_DEPRECATED_MASK = 3  /* Deprecated bits */
};

/* OpenFlow 1.3 specific flags for flow_mod messages. */
enum ofp13_flow_mod_flags {
    OFPFF13_NO_PKT_COUNTS = 1 << 3, /* Don't keep track of packet count. */
    OFPFF13_NO_BYT_COUNTS = 1 << 4  /* Don't keep track of byte count. */
};

/* Common header for all meter bands */
struct ofp13_meter_band_header {
    ovs_be16 type;       /* One of OFPMBT_*. */
    ovs_be16 len;        /* Length in bytes of this band. */
    ovs_be32 rate;       /* Rate for this band. */
    ovs_be32 burst_size; /* Size of bursts. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_header) == 12);

/* Meter configuration. OFPT_METER_MOD. */
struct ofp13_meter_mod {
    ovs_be16          command;      /* One of OFPMC_*. */
    ovs_be16          flags;        /* Set of OFPMF_*. */
    ovs_be32          meter_id;     /* Meter instance. */
    /* struct ofp13_meter_band_header bands[0];  The bands length is inferred
                                                 from the length field in the
                                                 header. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_mod) == 8);

/* Meter numbering. Flow meters can use any number up to OFPM_MAX. */
enum ofp13_meter {
    /* Last usable meter. */
    OFPM13_MAX        = 0xffff0000,
    /* Virtual meters. */
    OFPM13_SLOWPATH   = 0xfffffffd, /* Meter for slow datapath. */
    OFPM13_CONTROLLER = 0xfffffffe, /* Meter for controller connection. */
    OFPM13_ALL        = 0xffffffff, /* Represents all meters for stat requests
                                     commands. */
};

/* Meter commands */
enum ofp13_meter_mod_command {
    OFPMC13_ADD,           /* New meter. */
    OFPMC13_MODIFY,        /* Modify specified meter. */
    OFPMC13_DELETE         /* Delete specified meter. */
};

/* Meter configuration flags */
enum ofp13_meter_flags {
    OFPMF13_KBPS    = 1 << 0,   /* Rate value in kb/s (kilo-bit per second). */
    OFPMF13_PKTPS   = 1 << 1,   /* Rate value in packet/sec. */
    OFPMF13_BURST   = 1 << 2,   /* Do burst size. */
    OFPMF13_STATS   = 1 << 3    /* Collect statistics. */
};

/* Meter band types */
enum ofp13_meter_band_type {
    OFPMBT13_DROP         = 1,     /* Drop packet. */
    OFPMBT13_DSCP_REMARK  = 2,     /* Remark DSCP in the IP header. */
    OFPMBT13_EXPERIMENTER = 0xFFFF /* Experimenter meter band. */
};

/* OFPMBT_DROP band - drop packets */
struct ofp13_meter_band_drop {
    ovs_be16    type;        /* OFPMBT_DROP. */
    ovs_be16    len;         /* Length in bytes of this band. */
    ovs_be32    rate;        /* Rate for dropping packets. */
    ovs_be32    burst_size;  /* Size of bursts. */
    uint8_t     pad[4];
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_drop) == 16);

/* OFPMBT_DSCP_REMARK band - Remark DSCP in the IP header */
struct ofp13_meter_band_dscp_remark {
    ovs_be16    type;        /* OFPMBT_DSCP_REMARK. */
    ovs_be16    len;         /* Length in bytes of this band. */
    ovs_be32    rate;        /* Rate for remarking packets. */
    ovs_be32    burst_size;  /* Size of bursts. */
    uint8_t     prec_level;  /* Number of drop precedence level to add. */
    uint8_t     pad[3];
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_dscp_remark) == 16);

/* OFPMBT_EXPERIMENTER band - Write actions in action set */
struct ofp13_meter_band_experimenter {
    ovs_be16    type;        /* OFPMBT_EXPERIMENTER. */
    ovs_be16    len;         /* Length in bytes of this band. */
    ovs_be32    rate;        /* Rate for dropping packets. */
    ovs_be32    burst_size;  /* Size of bursts. */
    ovs_be32    experimenter; /* Experimenter ID which takes the same form as
                                 in struct ofp_experimenter_header. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_experimenter) == 16);

/* OF 1.3 adds MORE flag also for requests */
enum ofp13_multipart_request_flags {
    OFPMPF13_REQ_MORE = 1 << 0 /* More requests to follow. */
};

/* OF 1.3 splits table features off the ofp_table_stats */
/* Body of reply to OFPMP13_TABLE request. */
struct ofp13_table_stats {
    uint8_t  table_id;      /* Identifier of table. Lower numbered tables are
                               consulted first. */
    uint8_t  pad[3];        /* Align to 32-bits. */
    ovs_be32 active_count;  /* Number of active entries. */
    ovs_be64 lookup_count;  /* Number of packets looked up in table. */
    ovs_be64 matched_count; /* Number of packets that hit table. */
};
OFP_ASSERT(sizeof(struct ofp13_table_stats) == 24);

enum ofp15_table_features_command {
    OFPTFC15_REPLACE = 0,       /* Replace full pipeline. */
    OFPTFC15_MODIFY = 1,        /* Modify flow tables capabilities. */
    OFPTFC15_ENABLE = 2,        /* Enable flow tables in the pipeline. */
    OFPTFC15_DISABLE = 3,       /* Disable flow tables in pipeline. */
};

/* Body for ofp_multipart_request of type OFPMP_TABLE_FEATURES./
 * Body of reply to OFPMP_TABLE_FEATURES request. */
struct ofp13_table_features {
    ovs_be16 length;          /* Length is padded to 64 bits. */
    uint8_t table_id;         /* Identifier of table. Lower numbered tables
                                 are consulted first. */

    /* Added in OF1.5.  Earlier versions acted like OFPTFC15_REPLACE. */
    uint8_t command;          /* One of OFPTFC15_*. */

    uint8_t pad[4];           /* Align to 64-bits. */
    char name[OFP_MAX_TABLE_NAME_LEN];
    ovs_be64 metadata_match;  /* Bits of metadata table can match. */
    ovs_be64 metadata_write;  /* Bits of metadata table can write. */

    /* In OF1.3 this field was named 'config' and it was useless because OF1.3
     * did not define any OFPTC_* bits.
     *
     * OF1.4 renamed this field to 'capabilities' and added OFPTC14_EVICTION
     * and OFPTC14_VACANCY_EVENTS. */
    ovs_be32 capabilities;    /* Bitmap of OFPTC_* values */

    ovs_be32 max_entries;     /* Max number of entries supported. */

    /* Table Feature Property list */
    /* struct ofp13_table_feature_prop_header properties[0]; */
};
OFP_ASSERT(sizeof(struct ofp13_table_features) == 64);

/* Table Feature property types.
 * Low order bit cleared indicates a property for a regular Flow Entry.
 * Low order bit set indicates a property for the Table-Miss Flow Entry. */
enum ofp13_table_feature_prop_type {
    OFPTFPT13_INSTRUCTIONS         = 0, /* Instructions property. */
    OFPTFPT13_INSTRUCTIONS_MISS    = 1, /* Instructions for table-miss. */
    OFPTFPT13_NEXT_TABLES          = 2, /* Next Table property. */
    OFPTFPT13_NEXT_TABLES_MISS     = 3, /* Next Table for table-miss. */
    OFPTFPT13_WRITE_ACTIONS        = 4, /* Write Actions property. */
    OFPTFPT13_WRITE_ACTIONS_MISS   = 5, /* Write Actions for table-miss. */
    OFPTFPT13_APPLY_ACTIONS        = 6, /* Apply Actions property. */
    OFPTFPT13_APPLY_ACTIONS_MISS   = 7, /* Apply Actions for table-miss. */
    OFPTFPT13_MATCH                = 8, /* Match property. */
    OFPTFPT13_WILDCARDS            = 10, /* Wildcards property. */
    OFPTFPT13_WRITE_SETFIELD       = 12, /* Write Set-Field property. */
    OFPTFPT13_WRITE_SETFIELD_MISS  = 13, /* Write Set-Field for table-miss. */
    OFPTFPT13_APPLY_SETFIELD       = 14, /* Apply Set-Field property. */
    OFPTFPT13_APPLY_SETFIELD_MISS  = 15, /* Apply Set-Field for table-miss. */
    OFPTFPT13_EXPERIMENTER         = 0xFFFE, /* Experimenter property. */
    OFPTFPT13_EXPERIMENTER_MISS    = 0xFFFF, /* Experimenter for table-miss. */

    /* OpenFlow says that each of these properties must occur exactly once. */
#define OFPTFPT13_REQUIRED ((1u << OFPTFPT13_INSTRUCTIONS) |    \
                            (1u << OFPTFPT13_NEXT_TABLES) |     \
                            (1u << OFPTFPT13_WRITE_ACTIONS) |   \
                            (1u << OFPTFPT13_APPLY_ACTIONS) |   \
                            (1u << OFPTFPT13_MATCH) |           \
                            (1u << OFPTFPT13_WILDCARDS) |       \
                            (1u << OFPTFPT13_WRITE_SETFIELD) |  \
                            (1u << OFPTFPT13_APPLY_SETFIELD))

};

/* Body of reply to OFPMP13_PORT request. If a counter is unsupported, set
 * the field to all ones. */
struct ofp13_port_stats {
    struct ofp11_port_stats ps;
    ovs_be32 duration_sec;    /* Time port has been alive in seconds. */
    ovs_be32 duration_nsec;   /* Time port has been alive in nanoseconds
                                 beyond duration_sec. */
};
OFP_ASSERT(sizeof(struct ofp13_port_stats) == 112);

/* Body of reply to OFPMP13_QUEUE request */
struct ofp13_queue_stats {
    struct ofp11_queue_stats qs;
    ovs_be32 duration_sec;    /* Time queue has been alive in seconds. */
    ovs_be32 duration_nsec;   /* Time queue has been alive in nanoseconds
                                 beyond duration_sec. */
};
OFP_ASSERT(sizeof(struct ofp13_queue_stats) == 40);

/* Body of reply to OFPMP13_GROUP request */
struct ofp13_group_stats {
    struct ofp11_group_stats gs;
    ovs_be32 duration_sec;    /* Time group has been alive in seconds. */
    ovs_be32 duration_nsec;   /* Time group has been alive in nanoseconds
                                 beyond duration_sec. */
    /* struct ofp11_bucket_counter bucket_stats[]; */
};
OFP_ASSERT(sizeof(struct ofp13_group_stats) == 40);

/* Body of OFPMP13_METER and OFPMP13_METER_CONFIG requests. */
struct ofp13_meter_multipart_request {
    ovs_be32 meter_id;  /* Meter instance, or OFPM_ALL. */
    uint8_t pad[4];     /* Align to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_multipart_request) == 8);

/* Statistics for each meter band */
struct ofp13_meter_band_stats {
    ovs_be64    packet_band_count;      /* Number of packets in band. */
    ovs_be64    byte_band_count;        /* Number of bytes in band. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_stats) == 16);

/* Body of reply to OFPMP13_METER request. Meter statistics. */
struct ofp13_meter_stats {
    ovs_be32  meter_id;          /* Meter instance. */
    ovs_be16  len;               /* Length in bytes of this stats. */
    uint8_t   pad[6];
    ovs_be32  flow_count;        /* Number of flows bound to meter. */
    ovs_be64  packet_in_count;   /* Number of packets in input. */
    ovs_be64  byte_in_count;     /* Number of bytes in input. */
    ovs_be32  duration_sec;      /* Time meter has been alive in seconds. */
    ovs_be32  duration_nsec;     /* Time meter has been alive in nanoseconds
                                    beyond duration_sec. */
    struct ofp13_meter_band_stats band_stats[0];  /* The band_stats length is
                                             inferred from the length field. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_stats) == 40);

/* Body of reply to OFPMP13_METER_CONFIG request. Meter configuration. */
struct ofp13_meter_config {
    ovs_be16          length;       /* Length of this entry. */
    ovs_be16          flags;        /* Set of OFPMC_* that apply. */
    ovs_be32          meter_id;     /* Meter instance. */
    /* struct ofp13_meter_band_header bands[0];   The bands length is inferred
                                               from the length field. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_config) == 8);

/* Body of reply to OFPMP13_METER_FEATURES request. Meter features. */
struct ofp13_meter_features {
    ovs_be32   max_meter;     /* Maximum number of meters. */
    ovs_be32   band_types;    /* Bitmaps of OFPMBT13_* values supported. */
    ovs_be32   capabilities;  /* Bitmaps of "ofp13_meter_flags". */
    uint8_t    max_bands;     /* Maximum bands per meters */
    uint8_t    max_color;     /* Maximum color value */
    uint8_t    pad[2];
};
OFP_ASSERT(sizeof(struct ofp13_meter_features) == 16);

/* Asynchronous message configuration. */
/* The body of this is the same as nx_async_config */
/* OFPT_GET_ASYNC_REPLY or OFPT_SET_ASYNC. */
struct ofp13_async_config {
    ovs_be32 packet_in_mask[2];   /* Bitmasks of OFPR_* values. */
    ovs_be32 port_status_mask[2]; /* Bitmasks of OFPPR_* values. */
    ovs_be32 flow_removed_mask[2];/* Bitmasks of OFPRR_* values. */
};
OFP_ASSERT(sizeof(struct ofp13_async_config) == 24);

struct onf_flow_monitor_request {
    ovs_be32   id;            /* Controller-assigned ID for this monitor. */
    ovs_be16   flags;         /* ONFFMF_*. */
    ovs_be16   match_len;     /* Length of oxm_fields. */
    ovs_be32   out_port;      /* Required output port, if not OFPP_NONE. */
    uint8_t    table_id;      /* One table's ID or 0xff for all tables. */
    uint8_t    zeros[3];      /* Align to 64 bits (must be zero). */
    /* Followed by an ofp11_match structure. */
};
OFP_ASSERT(sizeof(struct onf_flow_monitor_request) == 16);

/* Header for experimenter requests and replies. */
struct onf_experimenter_header {
    struct ofp_header header;
    ovs_be32   vendor;        /* ONF_EXPERIMENTER_ID. */
    ovs_be32   subtype;       /* One of ONFT_*. */
};
OFP_ASSERT(sizeof(struct onf_experimenter_header) == 16);

enum onf_flow_monitor_msg_type {
    ONFT_FLOW_MONITOR_CANCEL = 1870,
    ONFT_FLOW_MONITOR_PAUSED = 1871,
    ONFT_FLOW_MONITOR_RESUMED = 1872
};

/* 'flags' bits in struct onf_flow_monitor_request. */
enum onf_flow_monitor_flags {
    /* When to send updates. */
    ONFFMF_INITIAL = 1 << 0,  /* Initially matching flows. */
    ONFFMF_ADD = 1 << 1,      /* New matching flows as they are added. */
    ONFFMF_DELETE = 1 << 2,   /* Old matching flows as they are removed. */
    ONFFMF_MODIFY = 1 << 3,   /* Matching flows as they are changed. */

    /* What to include in updates. */
    ONFFMF_ACTIONS = 1 << 4,  /* If set, actions are included. */
    ONFFMF_OWN = 1 << 5,      /* If set, include own changes in full. */
};

/* ONFST_FLOW_MONITOR reply header. */
struct onf_flow_update_header {
    ovs_be16   length;        /* Length of this entry. */
    ovs_be16   event;         /* One of ONFFME_*. */
    /* ...other data depending on 'event'... */
};
OFP_ASSERT(sizeof(struct onf_flow_update_header) == 4);

/* 'event' values in struct onf_flow_update_header. */
enum onf_flow_update_event {
    /* struct onf_flow_update_full. */
    ONFFME_ADDED = 0,         /* Flow was added. */
    ONFFME_DELETED = 1,       /* Flow was deleted. */
    ONFFME_MODIFIED = 2,      /* Flow (generally its actions) was changed. */

    /* struct onf_flow_update_abbrev. */
    ONFFME_ABBREV = 3,        /* Abbreviated reply. */
};

/* ONFST_FLOW_MONITOR reply for ONFFME_ADDED, ONFFME_DELETED, and
* ONFFME_MODIFIED. */
struct onf_flow_update_full {
    ovs_be16   length;        /* Length is 24. */
    ovs_be16   event;         /* One of ONFFME_*. */
    ovs_be16   reason;        /* OFPRR_* for ONFFME_DELETED, else zero. */
    ovs_be16   priority;      /* Priority of the entry. */
    ovs_be16   idle_timeout;  /* Number of seconds idle before expiration. */
    ovs_be16   hard_timeout;  /* Number of seconds before expiration. */
    ovs_be16   match_len;     /* Length of oxm_fields. */
    uint8_t    table_id;      /* ID of flow's table. */
    uint8_t    pad;           /* Reserved, currently zeroed. */
    ovs_be64   cookie;        /* Opaque controller-issued identifier. */
    /* Followed by:
     *   - Exactly match_len (possibly 0) bytes containing the oxm_fields, then
     *   - Exactly (match_len + 7)/8*8 - match_len (between 0 and 7) bytes of
     *     all-zero bytes, then
     *   - Instructions to fill out the remainder 'length' bytes (always a
     *     multiple of 8). If ONFFMF_ACTIONS was not specified, or 'event' is
     *     ONFFME_DELETED, no actions are included.
     */
};
OFP_ASSERT(sizeof(struct onf_flow_update_full) == 24);

/* ONFST_FLOW_MONITOR reply for ONFFME_ABBREV. */
struct onf_flow_update_abbrev {
    ovs_be16   length;     /* Length is 8. */
    ovs_be16   event;      /* ONFFME_ABBREV. */
    ovs_be32   xid;        /* Controller-specified xid from flow_mod. */
};
OFP_ASSERT(sizeof(struct onf_flow_update_abbrev) == 8);

#endif /* openflow/openflow-1.3.h */
