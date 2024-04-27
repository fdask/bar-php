<?php
class Config {
    public static $minBarWidth = 5;
    public static $maxBarWidth = 100;
    public static $minBarSpeed = 250;
    public static $maxBarSpeed = 1800;

    public static $defaultHealth = 10;
    
    public static $debug = true;

    public static function arrowLeft() {
		return json_decode('"\u2190"');
	}

	public static function arrowRight() {
		return json_decode('"\u2192"');
	}

	public static function arrowUp() {
		return json_decode('"\u2191"');
	}

	public static function arrowDown() {
		return json_decode('"\u2193"');
	}

    public static function keypressLeftArrow() {
        return chr(27) . chr(91) . chr(68);
    }

    public static function keypressShiftLeftArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(68);
    }

    public static function keypressRightArrow() {
        return chr(27) . chr(91) . chr(67);
    }

    public static function keypressShiftRightArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(67);
    }

    public static function keypressUpArrow() {
        return chr(27) . chr(91) . chr(65);
    }

    public static function keypressDownArrow() {
        return chr(27) . chr(91) . chr(66);
    }

    public static function waitForInput() {
        $input = '';

        $read = [STDIN];
        $write = null;
        $except = null;

        readline_callback_handler_install('', function() {});

        // Read characters from the command line one at a time until there aren't any more to read
        do {
            $input .= fgetc(STDIN);
        } while (stream_select($read, $write, $except, 0, 1));

        readline_callback_handler_remove();

        return $input;
    }

    public static function autoload($class) {
        $file = __DIR__ . "/" . str_replace('\\', '/', strtolower($class)) . ".inc";
        
        if (file_exists($file)) {
            require $file;
        } else {
            if (Config::$debug) {
                debug_print_backtrace();
            }
            
            die("Couldnt find file: $file\n");
        }
    }

    public static function draw($d) {
        if (!Config::$debug) {
            system("clear");
        }

        echo $d;
    }
}

spl_autoload_register("Config::autoload");