/*
Generated by LwipMibCompiler
*/

#include "lwip/snmp/snmp_opts.h"

#if LWIP_SNMP && LWIP_SNMP_V3 /* don't build if not configured for use in lwipopts.h */

#include "lwip/snmp/snmp_snmpv2_framework.h"
#include "lwip/snmp/snmp.h"
#include "lwip/snmp/snmp_core.h"
#include "lwip/snmp/snmp_scalar.h"
#include "lwip/snmp/snmp_table.h"
#include "lwip/snmp/snmpv3.h"
#include "snmpv3_priv.h"

#include "lwip/sys.h"

#include <string.h>

const struct snmp_obj_id usmNoAuthProtocol      = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 1, 1 } };
const struct snmp_obj_id usmHMACMD5AuthProtocol = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 1, 2 } };
const struct snmp_obj_id usmHMACSHAAuthProtocol = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 1, 3 } };
/* .4 sha-224
 * .5 sha-256
 * .6 sha-384
 * .7 sha-512
 */

const struct snmp_obj_id usmNoPrivProtocol  = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 2, 1 } };
const struct snmp_obj_id usmDESPrivProtocol = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 2, 2 } };
/* .3 3des-ede */
const struct snmp_obj_id usmAESPrivProtocol = { 10, { 1, 3, 6, 1, 6, 3, 10, 1, 2, 4 } };
/* .5 unknown
 * .6 unknown
 * .7 unknown
 */

/* TODO: where should this value come from? */
#define SNMP_FRAMEWORKMIB_SNMPENGINEMAXMESSAGESIZE 1500

/* --- snmpFrameworkMIBObjects 1.3.6.1.6.3.10.2 ----------------------------------------------------- */
static s16_t snmpengine_scalars_get_value(const struct snmp_scalar_array_node_def *node, void *value)
{
  const char *engineid;
  u8_t engineid_len;

  switch (node->oid) {
    case 1: /* snmpEngineID */
      snmpv3_get_engine_id(&engineid, &engineid_len);
      MEMCPY(value, engineid, engineid_len);
      return engineid_len;
    case 2: /* snmpEngineBoots */
      *(s32_t *)value = snmpv3_get_engine_boots_internal();
      return sizeof(s32_t);
    case 3: /* snmpEngineTime */
      *(s32_t *)value = snmpv3_get_engine_time_internal();
      return sizeof(s32_t);
    case 4: /* snmpEngineMaxMessageSize */
      *(s32_t *)value = SNMP_FRAMEWORKMIB_SNMPENGINEMAXMESSAGESIZE;
      return sizeof(s32_t);
    default:
      LWIP_DEBUGF(SNMP_MIB_DEBUG, ("snmpengine_scalars_get_value(): unknown id: %"S32_F"\n", node->oid));
      return 0;
  }
}

static const struct snmp_scalar_array_node_def snmpengine_scalars_nodes[] = {
  {1, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY}, /* snmpEngineID */
  {2, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY}, /* snmpEngineBoots */
  {3, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY}, /* snmpEngineTime */
  {4, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY}, /* snmpEngineMaxMessageSize */
};
static const struct snmp_scalar_array_node snmpengine_scalars = SNMP_SCALAR_CREATE_ARRAY_NODE(1, snmpengine_scalars_nodes, snmpengine_scalars_get_value, NULL, NULL);

static const struct snmp_node *const snmpframeworkmibobjects_subnodes[] = {
  &snmpengine_scalars.node.node
};
static const struct snmp_tree_node snmpframeworkmibobjects_treenode = SNMP_CREATE_TREE_NODE(2, snmpframeworkmibobjects_subnodes);

/* --- snmpFrameworkMIB  ----------------------------------------------------- */
static const struct snmp_node *const snmpframeworkmib_subnodes[] = {
  &snmpframeworkmibobjects_treenode.node
};
static const struct snmp_tree_node snmpframeworkmib_root = SNMP_CREATE_TREE_NODE(10, snmpframeworkmib_subnodes);
static const u32_t snmpframeworkmib_base_oid[] = {1, 3, 6, 1, 6, 3, 10};
const struct snmp_mib snmpframeworkmib = {snmpframeworkmib_base_oid, LWIP_ARRAYSIZE(snmpframeworkmib_base_oid), &snmpframeworkmib_root.node};

/* --- snmpFrameworkMIB  ----------------------------------------------------- */
#endif /* LWIP_SNMP */
