#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "stdout_exporter.h"  // Using an exporter that simply dumps span data to stdout.

#include "test_tracer/test_tracer.h"

#include <iostream>
#include <memory>

using example::StdoutExporter;
using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::AlwaysOnSampler;
using opentelemetry::sdk::trace::ParentOrElseSampler;
using opentelemetry::sdk::trace::ProbabilitySampler;
using opentelemetry::sdk::trace::Sampler;

namespace
{
void InitTracer(std::shared_ptr<Sampler> sampler)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor, sampler));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{

  auto always_on_sampler  = std::make_shared<AlwaysOnSampler>();
  auto always_off_sampler = std::make_shared<AlwaysOffSampler>();
  // A Parent-Or-Else sampler with a delegate AlwaysOn sampler.
  // This sampler requires SpanContext, so is not yet fully functional.
  auto parent_or_else_sampler = std::make_shared<ParentOrElseSampler>(always_on_sampler);
  // A Probability sampler with a 50% chance of sampling.
  // This sampler requires SpanContext, so is not yet fully functional.
  auto probabiltiy_sampler = std::make_shared<ProbabilitySampler>(0.5);

  std::cout << "Testing AlwaysOn Sampler...\n";
  // Set the sampler to AlwaysOn
  InitTracer(always_on_sampler);

  // The Tracer records every span
  TestTracer();

  std::cout << "\nTesting AlwaysOff Sampler...\n";
  // Set the sampler to AlwaysOn
  InitTracer(always_off_sampler);

  // The Tracer records nothing
  TestTracer();
}