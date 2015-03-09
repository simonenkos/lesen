#ifndef __LESEN__COMMON__LOGGER__
#define __LESEN__COMMON__LOGGER__

#include <fstream>
#include <memory>

/**
 * A log processor class, that holds pointer to a stream. Before usage
 * the logger should be initialized with 'initialize' method.
 */
class logger
{
   static std::shared_ptr<std::ofstream> log_ptr_;

public:
   logger() { }

   logger(const logger &  other) = delete;
   logger(const logger && other) = delete;

   logger & operator=(const logger &  other) = delete;
   logger & operator=(const logger && other) = delete;

   std::ostream & operator()()
   {
      if (!log_ptr_)
         throw std::logic_error("logger was not initialized");
      return * log_ptr_;
   }

   static bool initialize(const std::string& log_file)
   {
      if (!log_ptr_)
      {
         log_ptr_ = std::make_shared<std::ofstream>();

         if (!log_ptr_) return false;

         log_ptr_->open(log_file, std::ios::out);

         return log_ptr_->is_open();
      }
      return false;
   };
};

#define LOG(args) \
   logger()() << args << std::endl;

#endif /* __LESEN__COMMON__LOGGER__ */
