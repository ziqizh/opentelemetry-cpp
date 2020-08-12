#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::sdk::trace::Recordable;
using opentelemetry::sdk::trace::SpanData;

TEST(InMemorySpanData, AddRecordable)
{
  InMemorySpanData data(100);

  ASSERT_EQ(0, data.GetSpans().size());

  opentelemetry::nostd::span<std::unique_ptr<SpanData>> recordables(
      std::make_unique<SpanData>(), 1);

  for (auto &recordable : recordables)
  {
    auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));
    if (span != nullptr)
    {
      data.Add(std::move(span));
    }
  }

  // Consumes all spans in exporter
  ASSERT_EQ(1, data.GetSpans().size());

  ASSERT_EQ(0, data.GetSpans().size());
}