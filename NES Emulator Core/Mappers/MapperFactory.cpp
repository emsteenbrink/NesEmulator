#include "MapperFactory.h"
#include "Mapper_000.h"
#include "Mapper_001.h"
#include <cassert>


std::shared_ptr<Mapper> MapperFactory::CreateMapper(uint16_t mapperId, iNesConfiguration& configuration, const fs::path& filePath)
{
  switch (mapperId)
  {
  case 000: return std::make_shared<Mapper_000>(configuration, filePath);
  case 001: return std::make_shared<Mapper_001>(configuration, filePath);
  }
  assert(!"Unknown mapper ID");
  return nullptr;
}
