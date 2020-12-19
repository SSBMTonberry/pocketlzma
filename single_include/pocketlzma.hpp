//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_H
#define POCKETLZMA_POCKETLZMA_H


/*** Start of inlined file: pocketlzma_common.h ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_COMMON_H
#define POCKETLZMA_POCKETLZMA_COMMON_H

#include <vector>
#include <cstdint>

namespace plz
{
	const uint32_t DEFAULT_CHUNK_SIZE {1 << 16};
	const uint32_t DEFAULT_BUFFER_SIZE {1 << 22};
	enum class StatusCode
	{
		/*! All good! */
		Ok = 0,
		/*! Undefined error */
		Error = 999
	};
}
#endif //POCKETLZMA_POCKETLZMA_COMMON_H

/*** End of inlined file: pocketlzma_common.h ***/


/*** Start of inlined file: ICompressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_ICOMPRESSOR_HPP
#define POCKETLZMA_ICOMPRESSOR_HPP

namespace plz
{
	class ICompressor
	{
		public:
			virtual plz::StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) = 0;
	};
}

#endif //POCKETLZMA_ICOMPRESSOR_HPP

/*** End of inlined file: ICompressor.hpp ***/


/*** Start of inlined file: IDecompressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_IDECOMPRESSOR_HPP
#define POCKETLZMA_IDECOMPRESSOR_HPP

namespace plz
{
	class IDecompressor
	{
		public:
			virtual plz::StatusCode decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) = 0;
	};
}

#endif //POCKETLZMA_IDECOMPRESSOR_HPP

/*** End of inlined file: IDecompressor.hpp ***/


/*** Start of inlined file: Lzma2Compressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2COMPRESSOR_HPP
#define POCKETLZMA_LZMA2COMPRESSOR_HPP

namespace plz
{
	class Lzma2Compressor : ICompressor
	{
		public:
			Lzma2Compressor() = default;
			StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

		private:
			uint32_t m_bufferSize {DEFAULT_BUFFER_SIZE};
			uint32_t m_chunkSize {DEFAULT_CHUNK_SIZE};
	};

	StatusCode Lzma2Compressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
	{
		return StatusCode::Error;
	}
}

#endif //POCKETLZMA_LZMA2COMPRESSOR_HPP

/*** End of inlined file: Lzma2Compressor.hpp ***/

#endif //POCKETLZMA_POCKETLZMA_H

