#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

TEST(TracerProvider, GetTracer)
{
  std::shared_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  TracerProvider tf(processor);
  auto t1 = tf.GetTracer("test");
  auto t2 = tf.GetTracer("test");
  auto t3 = tf.GetTracer("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer = dynamic_cast<Tracer *>(t1.get());
  ASSERT_NE(nullptr, sdkTracer);
  ASSERT_EQ(processor, sdkTracer->GetProcessor());
}

TEST(TracerProvider, GetSampler)
{
  std::shared_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  // Create a TracerProvicer with a default AlwaysOnSampler.
  TracerProvider tf(processor);
  auto t1 = tf.GetSampler();
  auto t2 = tf.GetSampler();
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);

  // Should return the same sampler each time.
  ASSERT_EQ(t1, t2);

  // Should be AlwaysOnSampler
  ASSERT_EQ("AlwaysOnSampler", t2->GetDescription());
}

TEST(TracerProvider, SetSampler)
{
  std::shared_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  // Create a TracerProvicer with a default AlwaysOnSampler.
  TracerProvider tf(processor);

  // Check current Tracer sampler.
  auto tracer = tf.GetTracer("test");
  auto sdkTracer = dynamic_cast<Tracer *>(tracer.get());
  ASSERT_EQ("AlwaysOnSampler", sdkTracer->GetSampler()->GetDescription());

  // Set the sampler to a new AlwaysOffSampler.
  tf.SetSampler(std::make_shared<AlwaysOffSampler>());
  auto t1 = tf.GetSampler();

  // Check if the sampler in Tracer Provider has been updated.
  ASSERT_EQ("AlwaysOffSampler", t1->GetDescription());

  // Check if the sampler in Tracer has been updated.
  ASSERT_EQ("AlwaysOffSampler", sdkTracer->GetSampler()->GetDescription());
}