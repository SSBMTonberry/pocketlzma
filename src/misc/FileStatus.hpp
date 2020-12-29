//
// Created by robin on 29.12.2020.
//

#ifndef POCKETLZMA_FILESTATUS_HPP
#define POCKETLZMA_FILESTATUS_HPP

namespace plz
{
    class FileStatus
    {
        public:

            enum class Code
            {
                Ok = 0,
                FileWriteError = 100,
                FileWriteErrorBadBit = 101,
                FileWriteErrorFailBit = 102,

                FileReadError = 200,
                FileReadErrorBadBit = 201,
                FileReadErrorFailBit = 202
            };

            inline FileStatus() = default;
            inline FileStatus(FileStatus::Code status, int code, const std::string &exception, const std::string &category, const std::string &message);

            inline void set(FileStatus::Code status, int code, const std::string &exception, const std::string &category, const std::string &message);

            inline Code status() const;
            inline int code() const;
            inline const std::string &exception() const;
            inline const std::string &category() const;
            inline const std::string &message() const;

        private:
            Code m_status { Code::Ok };
            int m_code {0};
            std::string m_exception;
            std::string m_category;
            std::string m_message;

    };

    FileStatus::FileStatus(FileStatus::Code status, int code, const std::string &exception, const std::string &category, const std::string &message)
    {
        set(status, code, exception, category, message);
    }

    void FileStatus::set(FileStatus::Code status, int code, const std::string &exception, const std::string &category, const std::string &message)
    {
        m_status = status;
        m_code = code;
        m_exception = exception;
        m_category = category;
        m_message = message;
    }

    FileStatus::Code FileStatus::status() const
    {
        return m_status;
    }

    int FileStatus::code() const
    {
        return m_code;
    }

    const std::string &FileStatus::exception() const
    {
        return m_exception;
    }

    const std::string &FileStatus::category() const
    {
        return m_category;
    }

    const std::string &FileStatus::message() const
    {
        return m_message;
    }
}

#endif //POCKETLZMA_FILESTATUS_HPP
