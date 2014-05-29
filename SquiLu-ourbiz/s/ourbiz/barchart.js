dad.barChart = function()
{
    this.init = function(elm)
    {
        if(typeof elm === 'string') elm = document.getElementById(elm);
        this.canvas = elm;
    };
    this.canvas = null;
    this.getContext = function()
    {
        return this.canvas.getContext('2d');
    };
    this.label = "Bar Chart";
    this.label_align = -1; /*-1, 0, 1*/
    this._bars = [];
    this._bar_depth = 8;
    this._bar_update_count = 0;
    this._bar_num_format_decimals = 0;
    this._bar_color = "#6f6";

    this.bar_count = function()
    {
        return this._bars.length;
    };
    this.bar_add = function(bheader, bvalue, bcolor)
    {
        this._bars.push([bheader, bvalue, bcolor]);
    };
    this.getBar = function (idx)
    {
        return this._bars[idx];
    };

    this.get_bar_depth = function()
    {
        return this._bar_depth;
    };
    this.set_bar_depth = function(val)
    {
        this._bar_depth = val;
        this.update_bar_chart();
    };

    this.get_bar_num_format_decimals = function()
    {
        return this._bar_num_format_decimals;
    };
    this.set_bar_num_format_decimals = function(val)
    {
        this._bar_num_format_decimals = val;
        this.update_bar_chart();
    };

    this.bar_begin_update = function()
    {
        return ++this._bar_update_count;
    };
    this.bar_end_update = function()
    {
        if(this._bar_update_count === 0) throw "Barchart begin/end update out of sync !";
        if(--this._bar_update_count === 0) this.update_bar_chart();
    };

    this.update_bar_chart = function()
    {
        if(this._bar_update_count === 0) this.draw();
    };

    this.bar_clear = function()
    {
        this._bars.length = 0;
        this.update_bar_chart();
    };

    this._tmpSpan = document.createElement("span");
    this.getFontMetrics = function(text, font)
    {
        //var tm = this._tmpSpan
        //if(font) tm.font = font;
        //tm.textContent = text;
        //return { w: tm.offsetWidth, h: tm.offsetHeight};
        //return { w: this.getContext().measureText(text), h: 14};
				var ctx = this.getContext();
        var metrics = ctx.measureText(text);
				var hmetrics = ctx.measureText("L");
        metrics.height = hmetrics.width * 3;
        return metrics;
    };
    this.normalize_scale_units = function(oldScale)
    {
        var result = oldScale;
        if(result < 2) result = 2;
        else if(result <= 5) result = 5;
        else if(result <= 10) result = 10;
        else
        {
            var str = result.toString();
            var t = str.charAt(0) - '0';
            do
            {
                result = Math.floor(result / 10);
                t *= 10;
            }
            while(result >= 10);
            return t;
        }
        return result;
    };

    this.draw_mypolygon = function(x1, y1, x2, y2, x3, y3, x4, y4, line_color, fill_color)
    {
        var ctx = this.getContext();
        ctx.save();
        ctx.lineWidth = 1;
        ctx.fillStyle = fill_color;
        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2,y2);
        ctx.lineTo(x3,y3);
        ctx.lineTo(x4,y4);
        ctx.fill();
        ctx.stroke();
        ctx.closePath();

        ctx.strokeStyle = line_color;
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2,y2);
        ctx.lineTo(x3,y3);
        ctx.lineTo(x4,y4);
        ctx.stroke();
        //fl_line_style(FL_SOLID, 1, NULL);
        //fl_loop(x1, y1, x2, y2, x3, y3, x4, y4);
        ctx.restore();
    };

    this.draw_mybar = function(pbx, pby, pw, ph, pdepth, line_color, fill_color)
    {
        var lbx2 = pbx+pw;
        var lbx3 = lbx2+pdepth;
        var lby2 = pby+ph;
        var lby3 = pby-pdepth;
        // face rect
        this.draw_mypolygon(
            pbx, pby,
            lbx2, pby,
            lbx2, lby2,
            pbx, lby2,
            line_color,
            fill_color
        );
        // top polygon
        this.draw_mypolygon(
            pbx, pby,
            lbx2, pby,
            lbx3, lby3,
            pbx+pdepth, lby3,
            line_color,
            fill_color
        );
        // side polygon
        this.draw_mypolygon(
            lbx2, pby,
            lbx3, lby3,
            lbx3, lby2-pdepth,
            lbx2, lby2,
            line_color,
            fill_color
        );
    };

    // Fixed: Minus xStep bug (when x2 < x, original code bugs)
    // Fixed: Vertical line bug (when abs(x - x2) is zero, original code bugs because of NaN)
    //var CP = window.CanvasRenderingContext2D && CanvasRenderingContext2D.prototype;
    this.dashedLine = function(x, y, x2, y2, dashArray)
    {
        if(! dashArray) dashArray=[10,5];
        var dashCount = dashArray.length;
        var dx = (x2 - x);
        var dy = (y2 - y);
        var xSlope = (Math.abs(dx) > Math.abs(dy));
        var slope = (xSlope) ? dy / dx : dx / dy;

        this._canvas.moveTo(x, y);
        var distRemaining = Math.sqrt(dx * dx + dy * dy);
        var dashIndex = 0;
        while(distRemaining >= 0.1)
        {
            var dashLength = Math.min(distRemaining, dashArray[dashIndex % dashCount]);
            var step = Math.sqrt(dashLength * dashLength / (1 + slope * slope));
            if(xSlope)
            {
                if(dx < 0) step = -step;
                x += step;
                y += slope * step;
            }
            else
            {
                if(dy < 0) step = -step;
                x += slope * step;
                y += step;
            }
            this_canvas[(dashIndex % 2 === 0) ? 'lineTo' : 'moveTo'](x, y);
            distRemaining -= dashLength;
            dashIndex++;
        }
    };

    this.draw = function()
    {
        //set font size, color and clear
        //alert(this.canvas.parentNode.offsetWidth);
        //this.canvas.width = this.canvas.parentNode.offsetWidth * 0.96;
        //this.canvas.height = this.canvas.parentNode.offsetHeight * 0.96;
				//alert(this.canvas.width + ':'+this.canvas.height);

        var ctx = this.getContext();
				ctx.font = '10px verdana';
				//ctx.scale(2,2);
        var bar_depth = this._bar_depth;

        var color_white = "#ffffff";
        var color_black = "#000000";
        var color_dark3 = "#cccccc";
        var color_yellow = "yellow";

        ctx.fillStyle = color_white;
        ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

        var tm = this.getFontMetrics("W");
        var char_width = tm.width;
        var char_height = tm.height;
        var my_width = this.canvas.width;
        var my_height = this.canvas.height;

        var inner_padding = char_height;
        var box_x = 0;
        var box_y = char_height;

        var str;
        var nf_buf;

        var bars_box_x, bars_box_y, bars_box_width, bars_box_height;

        var box_label_width = inner_padding;
        var value_max = 0;

        var my_bars = this._bars;

        for(var i=0, bl = my_bars.length; i < bl; ++i)
        {
            var bar = my_bars[i];
            var max_value_str_width = ctx.measureText(bar[0]);
            if(max_value_str_width > box_label_width) box_label_width = max_value_str_width;
            if(bar[1]  > value_max) value_max = bar[1];
        }
        box_label_width += inner_padding *3;
        bars_box_x = box_x + box_label_width;
        bars_box_y = box_y + inner_padding;
        bars_box_height = my_height - inner_padding - 3 * char_height;
        bars_box_width = my_width - box_label_width - 3 * bar_depth - inner_padding;

        this.draw_mypolygon(
            bars_box_x, bars_box_y+bar_depth,
            bars_box_x, bars_box_y+bar_depth+bars_box_height,
            bars_box_x+bar_depth, bars_box_y+bars_box_height,
            bars_box_x+bar_depth, bars_box_y,
            color_black,
            color_yellow
        );
        this.draw_mypolygon(
            bars_box_x, bars_box_y+bar_depth+bars_box_height,
            bars_box_x+bars_box_width+bar_depth, bars_box_y+bar_depth+bars_box_height,
            bars_box_x+bars_box_width+bar_depth*2, bars_box_y+bars_box_height,
            bars_box_x+bar_depth, bars_box_y+bars_box_height,
            color_black,
            color_white
        );

        ctx.strokeStyle = color_black;
        ctx.lineWidth = 1;
        ctx.strokeRect(bars_box_x+bar_depth, bars_box_y, bars_box_width+bar_depth, bars_box_height+1);


        ctx.lineWidth = 1;
        ctx.moveTo(bars_box_x, bars_box_y+bar_depth);
        ctx.lineTo(bars_box_x, bars_box_y+bar_depth+bars_box_height);
        ctx.lineTo(bars_box_x+bars_box_width+bar_depth, bars_box_y+bar_depth+bars_box_height);
        ctx.stroke();

        var my_label = this.label;
        ctx.lineWidth = 1;
        ctx.fillStyle = "#0000ff"; // text color
        if(my_label) ctx.fillText(my_label, (my_width - ctx.measureText(my_label).width)/2, box_y);

        str = value_max.toString();
        var max_value_str_width, max_value_str_height;
        tm = this.getFontMetrics(str);
        max_value_str_width = tm.width;
        max_value_str_height = tm.height;

        var pixelPerUnit, nScaleLines, scaleUnits, itmp;
        if(value_max === 0)
        {
            pixelPerUnit = 1;
            nScaleLines = 1;
        }
        else
        {
            itmp = bars_box_width-max_value_str_width-bar_depth;
            pixelPerUnit = (itmp / value_max);
            nScaleLines = Math.ceil(itmp / (2*max_value_str_width));
        }

        if(nScaleLines === 0) scaleUnits = value_max +1;
        else scaleUnits = Math.floor(value_max / nScaleLines) +1;

        scaleUnits = this.normalize_scale_units(scaleUnits);
        if(scaleUnits === 0)	nScaleLines = 1;
        else nScaleLines = (value_max / scaleUnits)+1;

        ctx.strokeStyle = color_dark3;
        ctx.lineWidth = 1;

        var draw_scaleLine = function(dk,s)
        {
            var lx, ly, lx2, ly2;
            lx = bars_box_x + dk + bar_depth;
            ly = bars_box_y + bars_box_height;
            lx2 = bars_box_x + dk;
            ly2 = bars_box_y + bar_depth+bars_box_height;

            ctx.moveTo(lx, bars_box_y);
            ctx.lineTo(lx, ly);
            ctx.lineTo(lx2, ly2);
            ctx.stroke();

            ctx.moveTo(lx2, ly2);
            ctx.lineTo(lx2, ly2 + 2);
            ctx.stroke();

            ctx.save();
            ctx.fillStyle = color_black;
            ctx.fillText(s, bars_box_x + dk - half_max_value_str_width, bars_box_bottom+char_height);
            ctx.restore();
        };

        var half_max_value_str_width = Math.floor(max_value_str_width / 2);
        var bars_box_bottom = bars_box_y+bar_depth+bars_box_height+2;
        var str_zero = "0";
        if(value_max === 0)
        {
            var ik = Math.floor(bars_box_width / 2);
            draw_scaleLine(ik,str_zero);
        }
        else
        {
            ctx.fillStyle = color_black;
            ctx.fillText(str_zero, bars_box_x, bars_box_bottom+char_height);
            ctx.strokeStyle = color_dark3;
        }

        for(var ik=0; ik < nScaleLines; ik++)
        {
            var xs = Math.floor(scaleUnits*pixelPerUnit*ik);
            nf_buf = dad.number_format(ik*scaleUnits, this._bar_num_format_decimals);
            draw_scaleLine(xs, nf_buf);
        }

        var bar_height, bar_y;
        if(this._bars.length === 0) bar_height = 0;
        else bar_height = ((bars_box_height- bar_depth) / ((2.0*this._bars.length+1)));

        bars_box_bottom -= 2*bar_height + bar_depth;
        var ikl = 0;

        for(var i = 0, bl = my_bars.length; i < bl; ++i)
        {
            var bar = my_bars[i];
            bar_y = bars_box_bottom - (2*i*bar_height) ;

            var value = bar[1]; //bar value
            var bar_width = Math.floor(value*pixelPerUnit);

            this.draw_mybar(bars_box_x, bar_y, bar_width, bar_height, bar_depth, "#ccc", bar[2]);

            nf_buf = dad.number_format(value, this._bar_num_format_decimals);
            var rcenter = Math.floor(bar_y+(bar_height-bar_depth) / 2);

            ctx.strokeStyle = color_dark3;
            ctx.moveTo(bars_box_x+bar_width+bar_depth/2, rcenter);
            ctx.lineTo(bars_box_x+bar_width+bar_depth*2, rcenter);
            ctx.stroke();

            var str_width, str_height;
            tm = this.getFontMetrics(nf_buf);
            str_width = tm.width;
            str_height = tm.height;

            var rc_x = bars_box_x+bar_width+bar_depth*2;
            var rc_w = str_width+bar_depth;
            var rc_y = rcenter - (str_height/2);
            var rc_h = str_height * 1.1;

            ctx.beginPath();
            ctx.rect(rc_x, rc_y, rc_w, rc_h);
            ctx.fillStyle = color_yellow;
            ctx.fill();
            ctx.stroke();

            ctx.fillStyle = "black";
            ctx.fillText(nf_buf, rc_x+(bar_depth/2), rcenter + (str_height/3));

            str = bar[0]; //bar header
            tm = this.getFontMetrics(str);
            str_width = tm.width;
            str_height = tm.height;

            var str_x, str_y;
            //ctx.ctx.strokeStyle = "#000000";
            switch(this.label_align)
            {
            case 0: //FL_ALIGN_CENTER:
            {
                str_x = box_x+(box_label_width/2) - (str_width / 2);
                str_y = rcenter + (str_height/3);
            }
            break;
            case 1: //FL_ALIGN_RIGHT:
            {
                str_x = bars_box_x-str_width-inner_padding;
                str_y = rcenter + (str_height/3);
            }
            break;
            default:
            {
                str_x = box_x + inner_padding - 5;
                str_y = rcenter + (str_height/3);
            }
            }

            ctx.fillText(str, str_x, str_y);
        }
        //fl_line_style(0);
    };
};
