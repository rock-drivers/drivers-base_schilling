#ifndef _BASE_SCHILLING_DRIVER_HPP_
#define _BASE_SCHILLING_DRIVER_HPP_

#include <iodrivers_base/Driver.hpp>
#include <map>


namespace base_schilling
{
    class Driver : public iodrivers_base::Driver
    {
    public:
        Driver(int max_packet_size, bool extract_last = false);
	
	/** Tries to access the device at the provided URI
         *
         */
        void open(std::string const& uri);
	/** set Device Address
         */
	virtual void setAddress(uint const& address);
        /** set registers to be read
         */
	virtual void setRegs(std::vector<uint8_t> const& regs);
	/** get register value
	 */        
	virtual uint8_t getReg(uint8_t reg);
	/** sends Register Read command
	 */
        virtual void sendReadMsg();
	
	virtual void sendWriteMsg(int cmd, std::vector<uint8_t>* regs = NULL);
	
        
    protected:
        /** reading requested registers, storing into regMap  */
        virtual void read(int timeout);

        /** Extract a valid response from incoming Data
         *
         */
        virtual int extractPacket (uint8_t const *buffer, size_t buffer_size) const;
	
	virtual void setCS(char *cData);

	virtual void checkCS(const char *cData);
	
	std::map<uint8_t,uint8_t> mRegMap;
	
	uint mAddress;

    private:
    
    };

} // end namespace 

#endif 
