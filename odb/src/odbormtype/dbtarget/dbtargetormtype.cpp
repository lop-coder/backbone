#pragma once
#include "dbtargetormtype.h"
namespace ODBORM {
std::function<void(odb::callback_event e,
                   std::shared_ptr<ODBORM::DBTarget> target)>
    DBTarget::_eventFunc = nullptr;

unsigned long DBTarget::_idCnt = 0;
}  // namespace ODBORM
