<?php
/**
 * Z-Engine framework
 *
 * @copyright Copyright 2019, Lisachenko Alexander <lisachenko.it@gmail.com>
 *
 * This source file is subject to the license that is bundled
 * with this source code in the file LICENSE.
 *
 */
declare(strict_types=1);

namespace ZEngine\ClassExtension;

/**
 * Interface ObjectCompareZvalInterface allows to perform comparison of objects
 */
interface ObjectCompareZvalInterface
{
    /**
     * Performs comparison of given object with another value
     *
     * @param mixed $left left side of operation
     * @param mixed $right Right side of operation
     *
     * @return int Result of comparison: 1 is greater, -1 is less, 0 is equal
     */
    public static function __compare($left, $right): int;
}
