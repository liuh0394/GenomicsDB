/**
 * @file genomicsdb.h
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Omics Data Automation, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * Header file to an experimental general GenomicsDB query interface.
 *
 **/

#ifndef GENOMICSDB_H
#define GENOMICSDB_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

/** External export to override project visibility set to hidden */
#if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#  define GENOMICSDB_EXPORT __attribute__((visibility("default")))
#else
#  define GENOMICSDB_EXPORT
#endif

typedef std::pair<uint64_t, uint64_t> interval_t;

typedef struct genomic_interval_t {
  std::string contig_name;
  interval_t interval;
  genomic_interval_t(std::string contig_name, interval_t interval) {
    this->contig_name = contig_name;
    this->interval = interval;
  }
} genomic_interval_t;

typedef std::pair<std::string, std::string> genomic_field_t;

// Shadow internal data structures in variant.h for opaque-ness, to reduce
// re-compilation of api clients as new versions of the genomicsdb library are
// released.

// Shadow data structure to Variant - see src/main/cpp/include/genomicsdb/variant.h
// Similar to GAVariant in GA4GH API
typedef struct genomicsdb_variant_t genomicsdb_variant_t;

// Shadow data structure to VariantCall
// Similar to GACall in GA4GH API. Stores info about 1 CallSet/row for a given position
typedef struct genomicsdb_variant_call_t genomicsdb_variant_call_t;

// Data structure to query a subset of ranges, can represent either column or row ranges
// TODO: This should change to uint64_t!
typedef std::vector<std::pair<int64_t, int64_t>> genomicsdb_ranges_t;

// Default Segment Size in bytes = 10MB
#define DEFAULT_SEGMENT_SIZE 10u*1024u*1024u
#define SCAN_FULL {{0, INT64_MAX}}
#define ALL_ATTRIBUTES {}

GENOMICSDB_EXPORT std::string genomicsdb_version();

class GenomicsDBResults {
 public:
  //  GenomicsDBResults(std::vector<T>* results) : m_results(const_cast<std::vector<T>*>(results)) {};
  GenomicsDBResults(std::vector<genomicsdb_variant_t>* results) : m_results(results) {};
  GENOMICSDB_EXPORT std::size_t size() const noexcept;
  GENOMICSDB_EXPORT const genomicsdb_variant_t* at(std::size_t pos);
  inline const genomicsdb_variant_t* next() { return at(m_current_pos++); };
  GENOMICSDB_EXPORT void free();
 private:
  std::vector<genomicsdb_variant_t>* m_results;
  std::size_t m_current_pos=0;
};

// Forward Declarations for keeping Variant* classes opaque
class Variant;
class VariantCall;
class VariantQueryConfig;

/**
 * Experimental Query Interface to GenomicsDB for Arrays partitioned by columns
 * Concurrency support is provided via query json files for now - see 
 *     https://github.com/GenomicsDB/GenomicsDB/wiki/Querying-GenomicsDB#json-configuration-file-for-a-query
 *     https://github.com/GenomicsDB/GenomicsDB/wiki/MPI-with-GenomicsDB
 */
class GenomicsDB {
 public:
  /**
   * Constructor to the GenomicsDB Query API
   *   workspace
   *   callset_mapping_file
   *   vid_mapping_file
   *   reference_genome
   *   attributes, optional 
   *   segment_size, optional
   * Throws GenomicsDBException
   */
  GENOMICSDB_EXPORT GenomicsDB(const std::string& workspace,
             const std::string& callset_mapping_file,
             const std::string& vid_mapping_file,
             const std::string& reference_genome,
             const std::vector<std::string>attributes = ALL_ATTRIBUTES,
             const uint64_t segment_size = DEFAULT_SEGMENT_SIZE);

  /**
   * Constructor to the GenomicsDB Query API with configuration json files
   *   query_config_json_file
   *   loader_config_json_file, optional
   *   concurrency_rank, optional - if greater than 0, 
   *           the constraints(workspace, array, column and row ranges) are surmised
   *           using the rank as an index into their corresponding vectors.
   * Throws GenomicsDBException
   */
  GENOMICSDB_EXPORT GenomicsDB(const std::string& query_config_json_file,
             const std::string& loader_config_json_file = std::string(),
             const int concurrency_rank=0);

  /**
   * Destructor
   */
  GENOMICSDB_EXPORT ~GenomicsDB();

  /**
   * Query the array for variants constrained by the column and row ranges. 
   * Variants are similar to GAVariant in GA4GH API
   *   array
   *   column_ranges, optional
   *   row_ranges, optional
   */
  GENOMICSDB_EXPORT std::vector<genomicsdb_variant_t>* query_variants(const std::string& array,
                                                    genomicsdb_ranges_t column_ranges=SCAN_FULL,
                                                    genomicsdb_ranges_t row_ranges=SCAN_FULL);

  /**
   * Query using set configuration for variants. Useful when using parallelism paradigms(MPI, Intel TBB)
   * Variants are similar to GAVariant in GA4GH API
   */
  GENOMICSDB_EXPORT std::vector<genomicsdb_variant_t>*  query_variants();

  /**
   * Query the array for variants constrained by the column and row ranges. 
   * Variants are similar to GAVariant in GA4GH API
   *   array
   *   column_ranges, optional
   *   row_ranges, optional
   */
  GENOMICSDB_EXPORT GenomicsDBResults query_variants1(const std::string& array,
                                     genomicsdb_ranges_t column_ranges=SCAN_FULL,
                                     genomicsdb_ranges_t row_ranges=SCAN_FULL);


  /**
   * Query the array for variant calls constrained by the column and row ranges.
   * Variant Calls are similar to GACall in GA4GH API.
   *   array
   *   column_ranges, optional
   *   row_ranges, optional
   */
  GENOMICSDB_EXPORT std::vector<genomicsdb_variant_call_t>* query_variant_calls(const std::string& array,
                                                              genomicsdb_ranges_t column_ranges=SCAN_FULL,
                                                              genomicsdb_ranges_t row_ranges=SCAN_FULL);

  /**
   * Query using set configuration for variant calls. Useful when using parallelism paradigms(MPI, Intel TBB)
   * Variant Calls are similar to GACall in GA4GH API.
   */
  GENOMICSDB_EXPORT std::vector<genomicsdb_variant_call_t>* query_variant_calls();

  /*  GenomicsDBVariantCalls query_variant_calls1(const std::string& array,
                                              genomicsdb_ranges_t column_ranges=SCAN_FULL,
                                              genomicsdb_ranges_t row_ranges=SCAN_FULL);
  */
  /**
   * Utility template functions to extract information from Variant and VariantCall classes
   */
  GENOMICSDB_EXPORT interval_t get_interval(const genomicsdb_variant_t* variant);
  GENOMICSDB_EXPORT interval_t get_interval(const genomicsdb_variant_call_t* variant_call);

  GENOMICSDB_EXPORT genomic_interval_t get_genomic_interval(const genomicsdb_variant_t* variant);
  GENOMICSDB_EXPORT genomic_interval_t get_genomic_interval(const genomicsdb_variant_call_t* variant_call);

  GENOMICSDB_EXPORT std::vector<genomic_field_t> get_genomic_fields(const std::string& array, const genomicsdb_variant_t* variant);
  GENOMICSDB_EXPORT std::vector<genomic_field_t> get_genomic_fields(const std::string& array, const genomicsdb_variant_call_t* variant_call);

  /*  GENOMICSDB_EXPORT GenomicsDBVariantCalls get_variant_calls(const genomicsdb_variant_t* variant);*/
  
 private:
  std::vector<Variant>*  query_variants(const std::string& array, VariantQueryConfig *query_config);
  std::vector<VariantCall>* query_variant_calls(VariantQueryConfig *query_config);

  VariantQueryConfig* get_query_config_for(const std::string& array);

  void* m_storage_manager = nullptr; // Pointer to VariantStorageManager instance
  void* m_vid_mapper = nullptr;      // Pointer to VidMapper instance
  void* m_query_config = nullptr;    // Pointer to a base VariantQueryConfig instance

  int m_concurrency_rank = 0;

  //TODO: Get VariantFields to have names instead of indices at the point of building the data structure, so
  //      we don't have to maintain the following map
  std::map<std::string, VariantQueryConfig> m_query_configs_map; // Associate array names with VariantQueryConfig  
};

/**
 * GenomicsDBException is a catch all for all underlying genomicsdb library exceptions.
 */
class GenomicsDBException : public std::exception {
 public:
  GENOMICSDB_EXPORT GenomicsDBException(const std::string m="GenomicsDB Exception: ") : m_msg(m) {}
  GENOMICSDB_EXPORT ~GenomicsDBException() {}
  /** Returns the exception message. */
  GENOMICSDB_EXPORT const char* what() const noexcept {
    return m_msg.c_str();
  }
 private:
  std::string m_msg;
};

#endif /* GENOMICSDB_H */
