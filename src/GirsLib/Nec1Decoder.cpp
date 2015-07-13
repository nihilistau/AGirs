#include "Nec1Decoder.h"
#include "IrReader.h"
#include "IrReceiver.h"

//Nec1Decoder::~Nec1Decoder() {
//}
//{38.4k,564}<1,-1|1,-3>(16,-8,D:8,S:8,F:8,~F:8,1,^108m,(16,-4,1,^108m)*) [D:0..255,S:0..255=255-D,F:0..255]

String Nec1Decoder::toString() const {
  return !isValid() ? String()
    : ditto ? String(F("NEC1 ditto"))
    : String(F("NEC1 ")) + String(D) + String(F(" ")) + String(S) + String(F(" ")) + String(F);
}

unsigned int Nec1Decoder::decodeFlashGap(uint32_t flash, uint32_t gap) const {
    boolean result = getDuration(flash, 1);
    if (!result)
        return invalid;

    return getDuration(gap, 3) ? 1
            : getDuration(gap, 1) ? 0
            : invalid;
}

unsigned int Nec1Decoder::decode(const IrReader& irCapturer, unsigned int index) const {
    unsigned int sum = 0;
    for (int i = 7; i >= 0; i--) {
        unsigned int result = decodeFlashGap(irCapturer.getTime(2 * i + index), irCapturer.getTime(2 * i + 1 + index));
        if (result == invalid)
            return invalid;
        sum = (sum << 1) + result;
    }
    return sum;
}
#if 0
unsigned int Nec1Decoder::decode(const IrReceiver& irCapturer, unsigned int index) const {
    unsigned int sum = 0;
    for (int i = 7; i >= 0; i--) {
        unsigned int result = decodeFlashGap(irCapturer.getTime(2 * i + index), irCapturer.getTime(2 * i + 1 + index));
        if (result == invalid)
            return invalid;
        sum = (sum << 1) + result;
    }
    return sum;
}
#endif

boolean Nec1Decoder::tryDecode(const IRdecodeBase& iRdecodeBase, Stream& stream) {
    Nec1Decoder decoder(iRdecodeBase);
    return decoder.printDecode(stream);
}

boolean Nec1Decoder::tryDecode(const IrReader& irCapturer, Stream& stream) {
    Nec1Decoder decoder(irCapturer);
    return decoder.printDecode(stream);
}

unsigned int Nec1Decoder::decode(const IRdecodeBase& irCapturer, unsigned int index) const {
    unsigned int sum = 0;
    for (int i = 7; i >= 0; i--) {
        unsigned int result = decodeFlashGap(irCapturer.rawbuf[2 * i + index], irCapturer.rawbuf[2 * i + 1 + index]);
        if (result == invalid)
            return invalid;
        sum = (sum << 1) + result;
    }
    return sum;
}

uint16_t Nec1Decoder::decode(const IrReceiver::duration_t *data, uint16_t index) const {
    uint16_t sum = 0;
    for (int i = 7; i >= 0; i--) {
        uint16_t result = decodeFlashGap(data[2 * i + index], data[2 * i + 1 + index]);
        if (result == invalid)
            return invalid;
        sum = (sum << 1) + result;
    }
    return sum;
}

uint16_t Nec1Decoder::decode(const IrReceiverSampler& irReceiverSampler, uint16_t index) const {
    uint16_t sum = 0;
    for (int i = 7; i >= 0; i--) {
        uint16_t result = decodeFlashGap(irReceiverSampler.getDuration(2 * i + index), irReceiverSampler.getDuration(2 * i + 1 + index));
        if (result == invalid)
            return invalid;
        sum = (sum << 1) + result;
    }
    return sum;
}

//Nec1Decoder::Nec1Decoder(const IRdecodeBase& iRdecodeBase) : Decoder() {
//    init(iRdecodeBase.rawlen - 1, (const IrReceiver::duration_t *) (iRdecodeBase.rawbuf + 1));
//}

//Nec1Decoder::Nec1Decoder(uint16_t length, const IrReceiver::duration_t *data) : Decoder() {
//    init(length, data);
//}

Nec1Decoder::Nec1Decoder(const IrReceiverSampler &irReceiverSampler) {
    //super(irSequence);
    unsigned int index = 0;
    boolean success;
    if (irReceiverSampler.getDataLength() == 4) {
        success = getDuration(irReceiverSampler.getDuration(index++), 16U);
        if (!success)
            return;
        success = getDuration(irReceiverSampler.getDuration(index++), 4U);
        if (!success)
            return;
        success = getDuration(irReceiverSampler.getDuration(index++), 1U);
        if (!success)
            return;
        success = getEnding(irReceiverSampler.getDuration(index));
        if (!success)
            return;
        ditto = true;
        setValid(true);
    } else if (irReceiverSampler.rawlen == 34 * 2) {
        success = getDuration(irReceiverSampler.getDuration(index++), 16U);
        if (!success)
            return;
        success = getDuration(irReceiverSampler.getDuration(index++), 8U);
        if (!success)
            return;
        D = decode(irReceiverSampler, index);
        if (D == invalid)
            return;
        index += 16;
        S = decode(irReceiverSampler, index);
        if (S == invalid)
            return;
        index += 16;
        F = decode(irReceiverSampler, index);
        if (F == invalid)
            return;
        index += 16;
        int invF = decode(irReceiverSampler, index);
        if (invF < 0)
            return;
        if ((F ^ invF) != 0xFF)
            return;
        index += 16;

        success = getDuration(irReceiverSampler.getDuration(index++), 1U);
        if (!success)
            return;
        success = getEnding(irReceiverSampler.getDuration(index));
        if (!success)
            return;
        ditto = false;
        setValid(true);
    }
}
#if 0
Nec1Decoder::Nec1Decoder(const IrReceiver& irCapturer) : Decoder() {
    unsigned int index = 0;
    boolean success;
    if (irCapturer.getCaptureCount() == 4U) {
        success = getDuration(irCapturer.getTime(index++), 16U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 4U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 1U);
        if (!success)
            return;
        success = getEnding(irCapturer.getTime(index));
        if (!success)
            return;
        ditto = true;
        setValid(true);
    } else if (irCapturer.getCaptureCount() == 34U * 2U) {
        success = getDuration(irCapturer.getTime(index++), 16U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 8U);
        if (!success)
            return;
        D = decode(irCapturer, index);
        if (D == invalid)
            return;
        index += 16;
        S = decode(irCapturer, index);
        if (S == invalid)
            return;
        index += 16;
        F = decode(irCapturer, index);
        if (F == invalid)
            return;
        index += 16;
        int invF = decode(irCapturer, index);
        if (invF < 0)
            return;
        if ((F ^ invF) != 0xFF)
            return;
        index += 16;

        success = getDuration(irCapturer.getTime(index++), 1U);
        if (!success)
            return;
        success = getEnding(irCapturer.getTime(index));
        if (!success)
            return;
        ditto = false;
        setValid(true);
    }
}
#endif
Nec1Decoder::Nec1Decoder(const IrReader &irCapturer) : Decoder() {
    unsigned int index = 0;
    boolean success;
    if (irCapturer.getCaptureCount() == 4U) {
        success = getDuration(irCapturer.getTime(index++), 16U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 4U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 1U);
        if (!success)
            return;
        success = getEnding(irCapturer.getTime(index));
        if (!success)
            return;
        ditto = true;
        setValid(true);
    } else if (irCapturer.getCaptureCount() == 34U * 2U) {
        success = getDuration(irCapturer.getTime(index++), 16U);
        if (!success)
            return;
        success = getDuration(irCapturer.getTime(index++), 8U);
        if (!success)
            return;
        D = decode(irCapturer, index);
        if (D == invalid)
            return;
        index += 16;
        S = decode(irCapturer, index);
        if (S == invalid)
            return;
        index += 16;
        F = decode(irCapturer, index);
        if (F == invalid)
            return;
        index += 16;
        int invF = decode(irCapturer, index);
        if (invF < 0)
            return;
        if ((F ^ invF) != 0xFF)
            return;
        index += 16;

        success = getDuration(irCapturer.getTime(index++), 1U);
        if (!success)
            return;
        success = getEnding(irCapturer.getTime(index));
        if (!success)
            return;
        ditto = false;
        setValid(true);
    }
}

/*public static void main(String[] args) {
    try {
        IrSignal irSignal = new IrSignal("0000 006C 0022 0002 015B 00AD 0016 0016 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 06A4 015B 0057 0016 0E6C");
        IrSequence irSequence = irSignal.toModulatedIrSequence(3).noisify(20);
        IrSequence[] seqs = irSequence.chop(20000);
        for (IrSequence irs : seqs) {
            System.out.println(irs);
            Nec1Decoder decoder = new Nec1Decoder(irs);
            System.out.println(decoder);
        }

    } catch (IrpMasterException ex) {
        System.err.println(ex);
    }
}*/

