#include <Databases/IDatabase.h>
#include <Storages/System/attachSystemTables.h>

#include <Storages/System/StorageSystemAggregateFunctionCombinators.h>
#include <Storages/System/StorageSystemAsynchronousMetrics.h>
#include <Storages/System/StorageSystemBuildOptions.h>
#include <Storages/System/StorageSystemCollations.h>
#include <Storages/System/StorageSystemClusters.h>
#include <Storages/System/StorageSystemColumns.h>
#include <Storages/System/StorageSystemDatabases.h>
#include <Storages/System/StorageSystemDataTypeFamilies.h>
#include <Storages/System/StorageSystemDetachedParts.h>
#include <Storages/System/StorageSystemDictionaries.h>
#include <Storages/System/StorageSystemEvents.h>
#include <Storages/System/StorageSystemFormats.h>
#include <Storages/System/StorageSystemFunctions.h>
#include <Storages/System/StorageSystemGraphite.h>

#include <Storages/System/StorageSystemMacros.h>
#include <Storages/System/StorageSystemMerges.h>
#include <Storages/System/StorageSystemMetrics.h>
#include <Storages/System/StorageSystemModels.h>
#include <Storages/System/StorageSystemMutations.h>
#include <Storages/System/StorageSystemNumbers.h>
#include <Storages/System/StorageSystemOne.h>
#include <Storages/System/StorageSystemParts.h>
#include <Storages/System/StorageSystemPartsColumns.h>
#include <Storages/System/StorageSystemProcesses.h>
#include <Storages/System/StorageSystemReplicas.h>
#include <Storages/System/StorageSystemReplicationQueue.h>
#include <Storages/System/StorageSystemDistributionQueue.h>
#include <Storages/System/StorageSystemSettings.h>
#include <Storages/System/StorageSystemMergeTreeSettings.h>
#include <Storages/System/StorageSystemTableEngines.h>
#include <Storages/System/StorageSystemTableFunctions.h>
#include <Storages/System/StorageSystemTables.h>
#include <Storages/System/StorageSystemZooKeeper.h>
#include <Storages/System/StorageSystemContributors.h>
#if !defined(ARCADIA_BUILD)
    #include <Storages/System/StorageSystemLicenses.h>
#endif
#include <Storages/System/StorageSystemDisks.h>
#include <Storages/System/StorageSystemStoragePolicies.h>
#include <Storages/System/StorageSystemZeros.h>

#include <Storages/System/StorageSystemUsers.h>
#include <Storages/System/StorageSystemRoles.h>
#include <Storages/System/StorageSystemGrants.h>
#include <Storages/System/StorageSystemRoleGrants.h>
#include <Storages/System/StorageSystemCurrentRoles.h>
#include <Storages/System/StorageSystemEnabledRoles.h>
#include <Storages/System/StorageSystemSettingsProfiles.h>
#include <Storages/System/StorageSystemSettingsProfileElements.h>
#include <Storages/System/StorageSystemRowPolicies.h>
#include <Storages/System/StorageSystemQuotas.h>
#include <Storages/System/StorageSystemQuotaLimits.h>
#include <Storages/System/StorageSystemQuotaUsage.h>
#include <Storages/System/StorageSystemQuotasUsage.h>
#include <Storages/System/StorageSystemPrivileges.h>

#ifdef OS_LINUX
#include <Storages/System/StorageSystemStackTrace.h>
#endif


namespace DB
{

namespace
{

template<typename StorageT, typename... StorageArgs>
void attach(IDatabase & system_database, const String & table_name, StorageArgs && ... args)
{
    if (system_database.getUUID() == UUIDHelpers::Nil)
    {
        /// Attach to Ordinary database
        auto table_id = StorageID(DatabaseCatalog::SYSTEM_DATABASE, table_name);
        system_database.attachTable(table_name, StorageT::create(table_id, std::forward<StorageArgs>(args)...));
    }
    else
    {
        /// Attach to Atomic database
        /// NOTE: UUIDs are not persistent, but it's ok since no data are stored on disk for these storages
        /// and path is actually not used
        auto table_id = StorageID(DatabaseCatalog::SYSTEM_DATABASE, table_name, UUIDHelpers::generateV4());
        String path = "store/" + DatabaseCatalog::getPathForUUID(table_id.uuid);
        system_database.attachTable(table_name, StorageT::create(table_id, std::forward<StorageArgs>(args)...), path);
    }
}

}


///TODO allow store system tables in DatabaseAtomic
void attachSystemTablesLocal(IDatabase & system_database)
{
    attach<StorageSystemOne>(system_database, "one");
    attach<StorageSystemNumbers>(system_database, "numbers", false);
    attach<StorageSystemNumbers>(system_database, "numbers_mt", true);
    attach<StorageSystemZeros>(system_database, "zeros", false);
    attach<StorageSystemZeros>(system_database, "zeros_mt", true);
    attach<StorageSystemDatabases>(system_database, "databases");
    attach<StorageSystemTables>(system_database, "tables");
    attach<StorageSystemColumns>(system_database, "columns");
    attach<StorageSystemFunctions>(system_database, "functions");
    attach<StorageSystemEvents>(system_database, "events");
    attach<StorageSystemSettings>(system_database, "settings");
    attach<SystemMergeTreeSettings>(system_database, "merge_tree_settings");
    attach<StorageSystemBuildOptions>(system_database, "build_options");
    attach<StorageSystemFormats>(system_database, "formats");
    attach<StorageSystemTableFunctions>(system_database, "table_functions");
    attach<StorageSystemAggregateFunctionCombinators>(system_database, "aggregate_function_combinators");
    attach<StorageSystemDataTypeFamilies>(system_database, "data_type_families");
    attach<StorageSystemCollations>(system_database, "collations");
    attach<StorageSystemTableEngines>(system_database, "table_engines");
    attach<StorageSystemContributors>(system_database, "contributors");
    attach<StorageSystemUsers>(system_database, "users");
    attach<StorageSystemRoles>(system_database, "roles");
    attach<StorageSystemGrants>(system_database, "grants");
    attach<StorageSystemRoleGrants>(system_database, "role_grants");
    attach<StorageSystemCurrentRoles>(system_database, "current_roles");
    attach<StorageSystemEnabledRoles>(system_database, "enabled_roles");
    attach<StorageSystemSettingsProfiles>(system_database, "settings_profiles");
    attach<StorageSystemSettingsProfileElements>(system_database, "settings_profile_elements");
    attach<StorageSystemRowPolicies>(system_database, "row_policies");
    attach<StorageSystemQuotas>(system_database, "quotas");
    attach<StorageSystemQuotaLimits>(system_database, "quota_limits");
    attach<StorageSystemQuotaUsage>(system_database, "quota_usage");
    attach<StorageSystemQuotasUsage>(system_database, "quotas_usage");
    attach<StorageSystemPrivileges>(system_database, "privileges");

#if !defined(ARCADIA_BUILD)
    attach<StorageSystemLicenses>(system_database, "licenses");
#endif
#ifdef OS_LINUX
    attach<StorageSystemStackTrace>(system_database, "stack_trace");
#endif
}

void attachSystemTablesServer(IDatabase & system_database, bool has_zookeeper)
{
    attachSystemTablesLocal(system_database);

    attach<StorageSystemParts>(system_database, "parts");
    attach<StorageSystemDetachedParts>(system_database, "detached_parts");
    attach<StorageSystemPartsColumns>(system_database, "parts_columns");
    attach<StorageSystemDisks>(system_database, "disks");
    attach<StorageSystemStoragePolicies>(system_database, "storage_policies");
    attach<StorageSystemProcesses>(system_database, "processes");
    attach<StorageSystemMetrics>(system_database, "metrics");
    attach<StorageSystemMerges>(system_database, "merges");
    attach<StorageSystemMutations>(system_database, "mutations");
    attach<StorageSystemReplicas>(system_database, "replicas");
    attach<StorageSystemReplicationQueue>(system_database, "replication_queue");
    attach<StorageSystemDistributionQueue>(system_database, "distribution_queue");
    attach<StorageSystemDictionaries>(system_database, "dictionaries");
    attach<StorageSystemModels>(system_database, "models");
    attach<StorageSystemClusters>(system_database, "clusters");
    attach<StorageSystemGraphite>(system_database, "graphite_retentions");
    attach<StorageSystemMacros>(system_database, "macros");

    if (has_zookeeper)
        attach<StorageSystemZooKeeper>(system_database, "zookeeper");
}

void attachSystemTablesAsync(IDatabase & system_database, AsynchronousMetrics & async_metrics)
{
    attach<StorageSystemAsynchronousMetrics>(system_database, "asynchronous_metrics", async_metrics);
}

}
