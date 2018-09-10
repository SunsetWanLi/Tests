//
//  MITableViewCellAccessoryButton.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-8-25.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "MIButton.h"
#include "MITableView.h"

namespace z11 {
    class MGIndexPath;
    class MITableViewCellAccessoryButton : public MIButton
    {
    public:
        MITableViewCellAccessoryButton();
        virtual ~MITableViewCellAccessoryButton();
    public:
        byte_8 accessory_type;
        MGIndexPath row_in_table_view;
    };
} // namespace z11